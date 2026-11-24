# C0-T06 v1.0 收尾

- **线**：必做 | **计划用时**：1h | **前置**：C0-T05 | **依赖环境**：无
- **一句话目标**：别人 clone 下来，30 秒能看到结论，5 分钟能复现正确性测试

## 步骤
1. 【me】在 `results/README.md` 的结论表中补上 04（规模扫描）和 05（SIMD）两行，每个数字都能链接到对应的 CSV。
2. 【ai】确认 `results/README.md` 中的复现命令在 Linux（CI）和 Windows（x64 Native Tools）上都能跑通。
3. 【me】说"结束"，由归档流程完成战役收尾（ROADMAP 状态、时间线、进度条）。
4. 【me】打 tag 并发布 Release：`git tag -a v1.0 -m "..."`，Release 说明引用 `results/summary.md` 的摘要。
5. 【ai】冻结学习启动区：本仓库之后只作为成果仓库。把 `start/`、`AGENTS.md`、`GEMINI.md` 移到 `journey/archive/start-c0/`，首页"学习进度"和"我怎么开始学习"两节换成一句"C0 已完成，后续学习在 AI-Infra-Learning"，CI 改为只跑 smoke 测试；在 AI-Infra-Learning 的 ROADMAP 中把 C0 标为完成。

## 所有权
- `[me]`：结论表、tag 和 Release
- `[ai]`：复现命令的整理与验证

## 机器验收（开工前写死，AI 不得修改）
- [ ] GitHub 上存在 `v1.0` Release
- [ ] 打 tag 的那次提交上 CI 为绿色
- [ ] `results/README.md` 结论表中的每个数字都能链接到 CSV，且文件 ≤ 30 行
- [ ] `start/ROADMAP.md` 中 C0 的状态为"完成"（冻结前）
- [ ] 冻结后仓库根目录不再有 `start/`、`AGENTS.md`、`GEMINI.md`，CI 为绿色

## 闭卷要点
- 不考官。收尾时在 journey 时间线中写下本战役实际用时与计划用时之比，作为下个战役估时的参考。
