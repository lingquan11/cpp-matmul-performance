#!/usr/bin/env python3
"""同步首页的学习进度条和成果表，并检查仓库是否符合 start/RULES.md。

用法（仓库根目录）：
    python start/scripts/progress.py          # 更新 README.md，然后检查
    python start/scripts/progress.py --check  # 只检查，不写文件（CI 用）

数据源（唯一）：
    进度条  ← 当前战役 campaign.md 的"必做线"表（状态列）+ journey/log.md 的用时
              + start/ROADMAP.md 的战役表（状态列）
    成果表  ← results/README.md 中 <!-- results:start --> 与 <!-- results:end --> 之间的表格
README.md 中对应的块只由本脚本写入，不要手动修改。
"""
from __future__ import annotations

import argparse
import os
import re
import sys
import urllib.parse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUDGETS = {  # 文件 → 行数上限（start/RULES.md §1）
    "README.md": 40,
    "journey/README.md": 30,
    "results/README.md": 30,
    "start/STATE.md": 30,
}
LOG_LINE_MAX = 120
PASSED, DONE, ACTIVE = "通过", "完成", "进行中"
BAR_WIDTH = 20


def read(rel: str) -> str:
    return (ROOT / rel).read_text(encoding="utf-8")


def tables(text: str) -> list[tuple[list[str], list[list[str]]]]:
    """返回文本中所有 Markdown 表格：(表头, 行)。"""
    out, lines, i = [], text.splitlines(), 0
    while i < len(lines):
        if lines[i].lstrip().startswith("|") and i + 1 < len(lines) and re.match(r"^\s*\|[\s:|-]+\|\s*$", lines[i + 1]):
            cells = lambda s: [c.strip() for c in s.strip().strip("|").split("|")]
            header, rows, i = cells(lines[i]), [], i + 2
            while i < len(lines) and lines[i].lstrip().startswith("|"):
                rows.append(cells(lines[i]))
                i += 1
            out.append((header, rows))
        else:
            i += 1
    return out


def hours(cell: str) -> float:
    m = re.search(r"(\d+(?:\.\d+)?)\s*h", cell)
    return float(m.group(1)) if m else 0.0


def bar(frac: float) -> str:
    n = round(max(0.0, min(1.0, frac)) * BAR_WIDTH)
    return "█" * n + "░" * (BAR_WIDTH - n)


def current_campaign() -> Path:
    state = read("start/STATE.md")
    m = re.search(r"\((campaigns/[^)]+/campaign\.md)\)", state)
    if not m:
        sys.exit("start/STATE.md 的“战役”一行缺少指向 campaigns/<id>-<名称>/campaign.md 的链接")
    return Path("start") / m.group(1)


def campaign_info(path: Path) -> dict:
    text = read(str(path))
    title = re.search(r"^# 战役 (\S+)：(.+)$", text, re.M)
    deadline = re.search(r"\*\*周期\*\*：.*?→\s*(\d{4}-\d{2}-\d{2})", text)
    must = text.split("## 必做线", 1)[1].split("\n## ", 1)[0] if "## 必做线" in text else ""
    rows = []
    for header, body in tables(must):
        if "状态" in header and "计划用时" in header:
            hi, si = header.index("计划用时"), header.index("状态")
            rows = [(r[0], hours(r[hi]), r[si]) for r in body if len(r) > si]
    if not title or not rows:
        sys.exit(f"{path}：需要“# 战役 <id>：<名称>”标题和“## 必做线”表（含“计划用时”“状态”列）")
    return {"id": title.group(1), "name": title.group(2).strip(), "deadline": deadline.group(1) if deadline else "未设", "tasks": rows}


def used_hours(campaign_id: str) -> float:
    total = 0.0
    for line in read("journey/log.md").splitlines():
        parts = [p.strip() for p in line.split("|")]
        if len(parts) >= 3 and re.match(r"\d{4}-\d{2}-\d{2}$", parts[0]) and parts[2].startswith(campaign_id + "-"):
            total += hours(parts[1])
    return total


def roadmap_campaigns() -> list[tuple[str, str]]:
    for header, body in tables(read("start/ROADMAP.md")):
        if header and header[0] == "ID" and "状态" in header:
            si = header.index("状态")
            return [(r[0], r[si]) for r in body if re.match(r"^[A-Z]\d+$", r[0]) and len(r) > si]
    return []


def current_task_line() -> str:
    m = re.search(r"^- 当前任务：(.+)$", read("start/STATE.md"), re.M)
    return re.sub(r"\s*→.*$", "", m.group(1)).strip() if m else "未设"


def last_log_date() -> str:
    dates = re.findall(r"^(\d{4}-\d{2}-\d{2}) \|", read("journey/log.md"), re.M)
    return max(dates) if dates else "无"


def progress_block() -> str:
    c = campaign_info(current_campaign())
    plan = sum(h for _, h, _ in c["tasks"])
    done = sum(h for _, h, s in c["tasks"] if s == PASSED)
    passed = sum(1 for *_, s in c["tasks"] if s == PASSED)
    frac = done / plan if plan else 0.0
    fmt = lambda x: f"{x:g}"
    lines = [
        f"当前战役 {c['id']} {c['name']}",
        f"  {bar(frac)} {frac:4.0%}  {passed}/{len(c['tasks'])} 个必做任务通过 · 已用 {fmt(used_hours(c['id']))}h / 计划 {fmt(plan)}h · 截止 {c['deadline']}",
    ]
    route = roadmap_campaigns()
    if len(route) > 1:
        finished = sum(1 for _, s in route if s == DONE)
        cur = next((s for cid, s in route if cid == c["id"]), "")
        rfrac = (finished + (frac if cur == ACTIVE else 0)) / len(route)
        lines += [f"整条路线 {route[0][0]} → {route[-1][0]}", f"  {bar(rfrac)} {rfrac:4.0%}  {finished}/{len(route)} 个战役完成"]
    lines += [f"当前任务 {current_task_line()} · 最近学习 {last_log_date()}"]
    return "```\n" + "\n".join(lines) + "\n```"


def results_block() -> str:
    text = read("results/README.md")
    m = re.search(r"<!-- results:start -->\n(.*?)<!-- results:end -->", text, re.S)
    if not m:
        sys.exit("results/README.md 缺少 <!-- results:start --> / <!-- results:end --> 标记")

    def relink(match: re.Match) -> str:
        target = match.group(1)
        if re.match(r"^(https?:|#|mailto:|/)", target):
            return match.group(0)
        return "](" + os.path.normpath(os.path.join("results", target)).replace(os.sep, "/") + (
            "/" if target.endswith("/") else "") + ")"

    return re.sub(r"\]\(([^)\s]+)\)", relink, m.group(1).strip())


def render_readme() -> str:
    text = read("README.md")
    for name, block in (("progress", progress_block()), ("results", results_block())):
        pattern = re.compile(rf"(<!-- {name}:start -->\n)(?:.*?\n)?(<!-- {name}:end -->)", re.S)
        if not pattern.search(text):
            sys.exit(f"README.md 缺少 <!-- {name}:start --> / <!-- {name}:end --> 标记")
        text = pattern.sub(lambda m: m.group(1) + block + "\n" + m.group(2), text)
    return text


def check(expected_readme: str) -> list[str]:
    problems = []
    if read("README.md") != expected_readme:
        problems.append("README.md 的进度块或成果块没有同步：运行 python start/scripts/progress.py")
    budgets = dict(BUDGETS)
    for p in sorted((ROOT / "results/experiments").glob("*/README.md")):
        budgets[p.relative_to(ROOT).as_posix()] = 30
    for rel, limit in budgets.items():
        if (ROOT / rel).exists():
            n = len(read(rel).splitlines()) if rel != "README.md" else len(expected_readme.splitlines())
            if n > limit:
                problems.append(f"{rel}：{n} 行，超过上限 {limit}")
    for line in read("journey/log.md").splitlines():
        if re.match(r"\d{4}-\d{2}-\d{2} \|", line) and len(line) > LOG_LINE_MAX:
            problems.append(f"journey/log.md 有一行 {len(line)} 字，超过 {LOG_LINE_MAX}：{line[:30]}…")
    for md in sorted(ROOT.rglob("*.md")):
        rel = md.relative_to(ROOT).as_posix()
        if rel.startswith((".git/", "journey/archive/", "start/templates/")):
            continue
        for target in re.findall(r"\]\(([^)\s]+)\)", md.read_text(encoding="utf-8")):
            if re.match(r"^(https?:|#|mailto:)", target):
                continue
            if not (md.parent / urllib.parse.unquote(target.split("#")[0])).exists():
                problems.append(f"{rel}：失效链接 {target}")
    return problems


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--check", action="store_true", help="只检查，不写文件")
    args = parser.parse_args()
    expected = render_readme()
    if not args.check and read("README.md") != expected:
        (ROOT / "README.md").write_text(expected, encoding="utf-8")
        print("已更新 README.md 的进度块和成果块")
    problems = check(expected)
    for p in problems:
        print("✗ " + p)
    if not problems:
        print("✓ 进度、成果表、行数预算、日志长度、相对链接均通过")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
