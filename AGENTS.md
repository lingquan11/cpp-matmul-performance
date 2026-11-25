# AGENTS.md：给 GPT 和 Muse 的工作说明

这是我的学习仓库，分为三区（规则见 [start/RULES.md](start/RULES.md)）：`start/` 是我怎么开始，`journey/` 是学习过程，`results/` 是学习成果。学习分三栏：**阅读**（我自己读长文）、**编程**（GPT 带我写）、**动手**（Muse 负责）。

## 分工
| 谁 | 我说 | 做什么 | 依据 |
|---|---|---|---|
| **我** | — | 按当前战役 `campaign.md` 的阅读表读长文，不和 AI 讨论 | 阅读表 |
| **GPT**（编程导师） | "开始写代码" … "结束" | 带我写 `[me]` 代码、运行测试；结束时归档编程任务 | [coding-tutor.md](start/prompts/coding-tutor.md) → [coding-archiver.md](start/prompts/coding-archiver.md) |
| **Muse**（动手助理） | "准备环境" / "做 <H 任务ID>" | 装环境、配工具、跑实验与工具、整理结果，并归档 | [hands-on.md](start/prompts/hands-on.md) |
| **Muse** | "读完 / 在读 <R 编号>"、"做完 <S 编号>" | 记录阅读和支线练习进度（不提问、不考核） | [hands-on.md](start/prompts/hands-on.md) |

## 各自只改的文件
| | GPT | Muse |
|---|---|---|
| 任务卡 | 编程任务卡（`p*.md`） | 动手任务卡（`h*.md`） |
| 状态 | `campaign.md` 编程表；STATE"编程"小节 | `campaign.md` 阅读表和动手表；STATE"阅读""动手"小节；战役结束时的 ROADMAP |
| 记录 | `journey/coding-log.md`；`results/` 中的代码和代码 README | `journey/hands-on-log.md`；`results/` 中的数据、图、报告、`results/hw/environment.md` |
| 共享 | `journey/README.md` 时间线、`results/README.md` 结论表、首页进度块（由 `progress.py` 生成） | 同左 |

## 共同规则
- 开始前 `git pull`；任务卡不存在时先起草给我审核，验收不得弱于 `campaign.md` 中的要点。
- 不修改验收条件；不新建规则、计划或模板；不改已入库的原始数据；不代我写阅读笔记或实验结论。
- 归档后运行 `python start/scripts/progress.py` 直到全部 ✓，再提交推送。
- **共享文件只能从 learning-workflow 同步**：发现问题就告诉我，不要直接改。
