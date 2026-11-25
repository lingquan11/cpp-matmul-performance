#!/usr/bin/env python3
"""同步首页的学习进度条和成果表，并检查仓库是否符合 start/RULES.md。

用法（仓库根目录）：
    python start/scripts/progress.py          # 更新 README.md，然后检查
    python start/scripts/progress.py --check  # 只检查，不写文件（CI 用）

数据源（唯一）：
    进度条  ← 当前战役 campaign.md 的"阅读""编程""动手"三张表（状态列）+ 日志中的用时
              + start/ROADMAP.md 的战役表（状态列）
    成果表  ← results/README.md 中 <!-- results:start --> 与 <!-- results:end --> 之间的表格
README.md 中对应的块只由本脚本写入，不要手动修改。
"""
from __future__ import annotations

import argparse
import hashlib
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
LOGS = ["journey/coding-log.md", "journey/hands-on-log.md", "journey/log.md"]  # 编程（GPT）/ 动手与阅读（Muse）
REVIEW_DIRS = ["journey/reviews", "journey/theory/reviews"]  # Gemini 判定
LOCK = "start/WORKFLOW.lock"  # 共享文件的 sha256，由 learning-workflow 的 sync.py 写入
TASK_ID = re.compile(r"\b([A-Z]\d+-[RPHTS]\d+)\b")
PASSED, DONE, ACTIVE, READY = "通过", "完成", "进行中", "就绪"
FINISHED = {"通过", "读完", "完成", "就绪"}  # 三栏各自的"完成"状态
NOT_STARTED = {"未开始", "未读"}
LANES = [("阅读", "R"), ("编程", "P"), ("动手", "H")]
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


def log_lines() -> list[str]:
    return [line for rel in LOGS if (ROOT / rel).exists() for line in read(rel).splitlines()]


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
    def section_rows(name: str) -> list:
        if f"## {name}" not in text:
            return []
        body_text = text.split(f"## {name}", 1)[1].split("\n## ", 1)[0]
        for header, body in tables(body_text):
            hcol = next((h for h in ("计划用时", "预计用时") if h in header), None)
            if "状态" in header and hcol:
                hi, si = header.index(hcol), header.index("状态")
                return [(r[0], hours(r[hi]), r[si]) for r in body if len(r) > si]
        return []

    # 单线仓库用“必做线”；双线仓库用“理论线”和“实践线”
    if "## 必做线" in text:
        lanes = [("必做线", section_rows("必做线"))]
    elif any(f"## {n}" in text for n, _ in LANES):
        lanes = [(n, section_rows(n)) for n, _ in LANES]
    else:
        lanes = [(n, section_rows(n)) for n in ("理论线", "实践线")]
    lanes = [(n, r) for n, r in lanes if r]
    rows = [t for _, r in lanes for t in r]
    if not title or not rows:
        sys.exit(f"{path}：需要“# 战役 <id>：<名称>”标题，以及“## 阅读”“## 编程”“## 动手”表（含“计划用时”或“预计用时”和“状态”列）")
    env = []  # 环境任务（与 Muse 协作，不计入学习进度）
    if "## 环境准备" in text:
        for header, body in tables(text.split("## 环境准备", 1)[1].split("\n## ", 1)[0]):
            if "状态" in header:
                si = header.index("状态")
                env = [(r[0], r[si]) for r in body if len(r) > si]
    return {"id": title.group(1), "name": title.group(2).strip(), "deadline": deadline.group(1) if deadline else "未设", "tasks": rows, "lanes": lanes, "env": env}


def used_hours(campaign_id: str, kind: str = "learn") -> float:
    """本战役已用时。kind：learn = 全部学习任务；env = 环境任务（S）；R / P / H / T = 阅读 / 编程 / 动手 / 理论任务。"""
    total = 0.0
    for line in log_lines():
        parts = [p.strip() for p in line.split("|")]
        if len(parts) >= 3 and re.match(r"\d{4}-\d{2}-\d{2}$", parts[0]) and parts[2].startswith(campaign_id + "-"):
            letter = parts[2][len(campaign_id) + 1:][:1]
            if (kind == "learn" and letter != "S") or (kind == "env" and letter == "S") or letter == kind:
                total += hours(parts[1])
    return total


def roadmap_campaigns() -> list[tuple[str, str]]:
    for header, body in tables(read("start/ROADMAP.md")):
        if header and header[0] == "ID" and "状态" in header:
            si = header.index("状态")
            return [(r[0], r[si]) for r in body if re.match(r"^[A-Z]\d+$", r[0]) and len(r) > si]
    return []


def current_task_lines() -> list[str]:
    found = re.findall(r"^- 当前(阅读|编程任务|动手任务|理论任务|实践任务|任务)：(.+)$", read("start/STATE.md"), re.M)
    return [f"当前{label} " + re.sub(r"\s*→.*$", "", text).strip() for label, text in found] or ["当前任务 未设"]


def last_log_date() -> str:
    dates = re.findall(r"^(\d{4}-\d{2}-\d{2}) \|", "\n".join(log_lines()), re.M)
    return max(dates) if dates else "无"


def progress_block() -> str:
    c = campaign_info(current_campaign())
    plan = sum(h for _, h, _ in c["tasks"])
    done = sum(h for _, h, s in c["tasks"] if s in FINISHED)
    passed = sum(1 for *_, s in c["tasks"] if s in FINISHED)
    frac = done / plan if plan else 0.0
    fmt = lambda x: f"{round(x, 1):g}"  # 用时显示一位小数
    if len(c["lanes"]) == 1:
        lines = [
            f"当前战役 {c['id']} {c['name']}",
            f"  {bar(frac)} {frac:4.0%}  {passed}/{len(c['tasks'])} 个必做任务通过 · 已用 {fmt(used_hours(c['id']))}h / 计划 {fmt(plan)}h · 截止 {c['deadline']}",
        ]
    else:
        lines = [f"当前战役 {c['id']} {c['name']} · 截止 {c['deadline']}"]
        for name, rows in c["lanes"]:
            lp = sum(h for _, h, _ in rows)
            lf = sum(h for _, h, s in rows if s in FINISHED) / lp if lp else 0.0
            ln = sum(1 for *_, s in rows if s in FINISHED)
            lines.append(f"  {name} {bar(lf)} {lf:4.0%}  {ln}/{len(rows)} 项完成 · 已用 {fmt(used_hours(c['id'], dict(LANES + [('理论线', 'T'), ('实践线', 'P')]).get(name, 'learn')))}h / 计划 {fmt(lp)}h")
    route = roadmap_campaigns()
    if len(route) > 1:
        finished = sum(1 for _, s in route if s == DONE)
        cur = next((s for cid, s in route if cid == c["id"]), "")
        rfrac = (finished + (frac if cur == ACTIVE else 0)) / len(route)
        lines += [f"整条路线 {route[0][0]} → {route[-1][0]}", f"  {bar(rfrac)} {rfrac:4.0%}  {finished}/{len(route)} 个战役完成"]
    if c["env"]:
        ready = sum(1 for _, s in c["env"] if s == READY)
        lines += [f"环境准备 {ready}/{len(c['env'])} 就绪（与 Muse 协作，不计入学习进度）· 已用 {fmt(used_hours(c['id'], 'env'))}h"]
    lines += current_task_lines() + [f"最近学习 {last_log_date()}"]
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


def consistency() -> list[str]:
    """归档漏步检查：状态、日志、STATE、考核结果之间是否一致。"""
    problems = []
    c = campaign_info(current_campaign())
    status = {tid: s for tid, _, s in c["tasks"]} | dict(c["env"])
    for line in current_task_lines():
        for tid in TASK_ID.findall(line):
            if tid not in status:
                problems.append(f"start/STATE.md 指向的 {tid} 不在当前战役的任务表中")
    logged = set()
    for line in log_lines():
        parts = [p.strip() for p in line.split("|")]
        if len(parts) >= 3 and re.match(r"\d{4}-\d{2}-\d{2}$", parts[0]):
            logged.update(TASK_ID.findall(parts[2]))
    for tid in sorted(logged):
        if status.get(tid) in NOT_STARTED:
            problems.append(f"{tid} 已有日志，但任务表中的状态仍是“{status[tid]}”")
    for d in REVIEW_DIRS:
        for f in sorted((ROOT / d).glob("*.md")) if (ROOT / d).exists() else []:
            verdicts = re.findall(r"^结论:\s*(通过|不通过)", f.read_text(encoding="utf-8"), re.M)
            tid = f.stem
            if verdicts and tid in status:
                if verdicts[-1] == "通过" and status[tid] != PASSED:
                    problems.append(f"{tid} 的最新考核结论是“通过”，但任务表中的状态是“{status[tid]}”")
                if verdicts[-1] == "不通过" and status[tid] == PASSED:
                    problems.append(f"{tid} 的最新考核结论是“不通过”，但任务表中的状态是“通过”")
    return problems


def lock_problems() -> list[str]:
    """共享文件是否与 learning-workflow 同步时一致（防止被直接修改）。"""
    if not (ROOT / LOCK).exists():
        return []
    problems = []
    for line in read(LOCK).splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        digest, rel = line.split(maxsplit=1)
        path = ROOT / rel
        if not path.exists():
            problems.append(f"共享文件缺失：{rel}（用 learning-workflow 的 sync.py 同步）")
        elif hashlib.sha256(path.read_bytes()).hexdigest() != digest:
            problems.append(f"共享文件被直接修改：{rel}（只能用 learning-workflow 的 sync.py 同步）")
    return problems


def check(expected_readme: str) -> list[str]:
    problems = lock_problems() + consistency()
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
    for line in log_lines():
        if re.match(r"\d{4}-\d{2}-\d{2} \|", line) and len(line) > LOG_LINE_MAX:
            problems.append(f"日志有一行 {len(line)} 字，超过 {LOG_LINE_MAX}：{line[:30]}…")
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
