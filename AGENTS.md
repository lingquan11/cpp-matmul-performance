# AGENTS.md：给 Codex 的工作说明

这是我的学习仓库，分为三区（规则见 [start/RULES.md](start/RULES.md)）：`start/` 是我怎么开始，`journey/` 是学习过程，`results/` 是学习成果。一次学习从头到尾在**同一个对话**里完成。

## 我说"开始学习"
1. `git pull`；读 `start/RULES.md`、`start/STATE.md`、STATE 指向的任务卡、`journey/log.md` 的最后 5 行。
2. 如果当前任务卡还不存在（只有 `campaign.md` 中的一行）：按 `start/templates/task.md` 起草，验收不得弱于 `campaign.md` 中的验收要点。给我审核，我确认后提交，再开始学习。
3. 按 [start/prompts/tutor.md](start/prompts/tutor.md) 教学。

## 学习过程中
- 机器验收的命令由你执行，原样保留输出。**计时测量**只在我明确说"开始测量"时才运行。
- 任务卡中标 `[me]` 的代码你不写，按 tutor.md 的提示梯度引导我自己写。

## 任务的机器验收全部通过
按 [start/prompts/examiner.md](start/prompts/examiner.md) 执行（它自己决定是否需要考核、是否先提醒切换模型）。

## 我说"结束"、"今天到这"、"学完了"（或考核结束后）
按 [start/prompts/archiver.md](start/prompts/archiver.md) 归档：同步所有联动文件 → 运行 `python start/scripts/progress.py` 直到全部 ✓ → 提交并推送 → 用 3–5 行告诉我改了什么、进度条现在是多少、下次从哪里开始。

## 其他时候
作为仓库助手，只做我要求的事。不新建规则、计划或模板文件；不修改任务卡中的验收条件；不改动已入库的原始数据。
