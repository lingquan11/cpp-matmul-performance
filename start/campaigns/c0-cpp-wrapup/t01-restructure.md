# T01 三区重构：start / journey / results

- **线**：必做 | **计划用时**：1.5h | **前置**：无
- **一句话目标**：仓库分成"我怎么开始"、"学习过程"、"学习成果"三区；给别人看的部分精简到几分钟就能读完

## 步骤
1. 【ai，已完成】`git mv` 迁移：现行程序、测试、脚本和三个实验 → `results/`；Day 1–8 的笔记 → `journey/notes/`（按主题改名）；其余全部原样移到 `journey/archive/`；工作流 → `start/`。
2. 【ai，已完成】为三个实验写精简 README，数字用脚本与原始 CSV 逐个核对；更新 CI、smoke.sh、include 和脚本路径；`blocking.cpp` 的字节保持不变（实验 03 记录了它的 SHA-256）。
3. 【me】打开 GitHub 首页读一遍：5 分钟内能看懂"我怎么开始 / 学了什么 / 学成了什么"吗？有看不懂的地方就记下来。
4. 【me】在 `journey/log.md` 追加一行：这次重构为什么做、你核对的结论。以 `[me]` 前缀提交。

## 所有权
- `[me]`：第 3、4 步
- `[ai]`：文件迁移、精简 README、路径修复

## 机器验收（开工前写死，AI 不得修改）
- [ ] `bash results/tests/smoke.sh` 通过，GitHub Actions 的 compile 工作流为绿色
- [ ] `README.md` ≤ 40 行，`journey/README.md`、`results/README.md` 和每个实验 README 各 ≤ 30 行
- [ ] 归档区外没有失效的相对链接
- [ ] git 中存在一个 `[me]` 提交（第 4 步）

## 闭卷要点
- 不考官。

## 卡住时
- 首页有看不懂的地方：直接在 log 里写出来，下次让 AI 按 RULES §1 修改。
