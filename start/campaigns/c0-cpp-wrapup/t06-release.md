# T06 v1.0 收尾

- **线**：必做 | **计划用时**：1h | **前置**：T05
- **一句话目标**：别人 clone 下来，30 秒能看到结论，5 分钟能复现正确性测试

## 步骤
1. 【me】在 `results/README.md` 的结论表中补上 04（规模扫描）和 05（SIMD）两行，每个数字都能链接到对应的 CSV。
2. 【ai】确认 `results/README.md` 中的复现命令在 Linux（CI）和 Windows（x64 Native Tools）上都能跑通。
3. 【me】把 `campaign.md` 中所有必做项的状态更新为最终结果；在 `journey/README.md` 的时间线中加一行。
4. 【me】打 tag 并发布 Release：`git tag -a v1.0 -m "..."`，Release 说明引用 `results/summary.md` 的摘要。
5. 【me】用 `reviewer.md` 做战役复盘，结果存为 `journey/reviews/c0-retro.md`。

## 所有权
- `[me]`：结论表、tag 和 Release、复盘
- `[ai]`：复现命令的整理与验证

## 机器验收（开工前写死，AI 不得修改）
- [ ] GitHub 上存在 `v1.0` Release
- [ ] 打 tag 的那次提交上 CI 为绿色
- [ ] `results/README.md` 结论表中的每个数字都能链接到 CSV，且文件 ≤ 30 行
- [ ] `journey/reviews/c0-retro.md` 存在

## 闭卷要点
- 不考官。复盘时回答：这个战役的实际用时和计划用时之比是多少？下个战役的计划要乘以多少？
