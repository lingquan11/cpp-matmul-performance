# 还缺的材料

1. 截图转录那 15 条的批量执行源码还没上传。重复次数是 585 / 89 / 9，和程序导出不是同一份输出。[`templates/gpt/day04-benchmark-template.cpp`](../../../templates/gpt/day04-benchmark-template.cpp) 是 GPT 模板，不是那份执行稿。
2. 程序导出已经在 [`day04-local-program-export.csv`](day04-local-program-export.csv)。与它一起提交的源码是 [`code_history/day04/V07_user_ikj_program_export.cpp`](../../../code_history/day04/V07_user_ikj_program_export.cpp)。表头和输出文件名一致，只含 `matmul_ikj`。耗时不在源码里，没有重跑核对，所以不写成已经逐行复现。
3. Visual Studio 只记到用户说的 2026、18.x。更细的编译器版本号未记录。
4. 两份 15 条都只检查了 `C[0][0]` 和校验和，没有逐元素对照。
5. 128 阶偏高样本的原因暂留，以后再研究。原值不删。

测试记录是留着的。这里只记还缺的材料和还没定的原因。
