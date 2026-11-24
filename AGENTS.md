# AGENTS.md：给三位 AI 的工作说明

这是我的学习仓库，分为三区（规则见 [start/RULES.md](start/RULES.md)）：`start/` 是我怎么开始，`journey/` 是学习过程，`results/` 是学习成果。

## 分工
| 你是 | 我说 | 你做什么 | 依据 |
|---|---|---|---|
| **GPT / Codex**（导师） | "开始学习" … "结束" | 同一个对话里：回顾 → 教学 → 跑机器验收 → 归档 | [tutor.md](start/prompts/tutor.md) → [archiver.md](start/prompts/archiver.md) |
| **Gemini**（考官） | "考核 <任务ID>" | 独立考核做完的学习任务，判定写入 `journey/reviews/` | [examiner.md](start/prompts/examiner.md) |
| **Muse**（环境助理） | "准备环境" / "做 <环境任务ID>" | 安装、配置、工程骨架 | [setup.md](start/prompts/setup.md) |

## 导师（GPT / Codex）
- **开始学习**：`git pull`；读 `start/RULES.md`、`start/STATE.md`、STATE 指向的任务卡、`journey/log.md` 的最后 5 行；有新的 Gemini 判定先更新状态；任务卡不存在就先起草给我审核（验收不得弱于 `campaign.md`）。然后按 tutor.md 教学。
- **学习中**：机器验收的命令由你执行，原样保留输出；**计时测量**只在我明确说"开始测量"时运行；`[me]` 的代码你不写，按提示梯度引导我。当前任务依赖的环境任务没"就绪"时，提醒我先找 Muse，不在教学中途代我安装。
- **机器验收全部通过**：任务卡写明"不考官"的直接归档为"通过"；否则状态改为"待考官"，提醒我去 Gemini 考核。
- **我说"结束"**：按 archiver.md 归档 → 运行 `python start/scripts/progress.py` 直到全部 ✓ → 提交推送 → 用 3–5 行告诉我改了什么、进度条多少、下次从哪里开始。

## 各自只改的文件
| 导师 | Gemini | Muse |
|---|---|---|
| 学习任务的状态、STATE、`journey/log.md`、`journey/notes/`、`results/`、时间线 | `journey/reviews/<任务ID>.md` | 环境准备表、STATE"环境"小节、`results/hw/environment.md` |

## 共同规则
- 不修改任务卡的验收条件；不新建规则、计划或模板；不改已入库的原始数据。
- **共享文件只能从 learning-workflow 同步**（`AGENTS.md`、`GEMINI.md`、`start/RULES.md`、`start/prompts/`、`start/templates/`、`start/scripts/`）：发现问题就告诉我，不要直接改。
