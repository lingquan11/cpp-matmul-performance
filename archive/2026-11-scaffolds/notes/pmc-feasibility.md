# 硬件计数器探路（AI 维护清单，尚未实测）

**累计约 45 分钟硬停。一旦驱动无法加载，或需要关闭 Secure Boot、VBS、内存完整性，立即放弃 Plan A 转 Plan B；不更改系统安全设置。**

- [ ] 打开 `msinfo32`，只查看并记录基于虚拟化的安全状态和内存完整性状态；截图/文字：待填。
- [ ] `wpr -pmcsources` 查看系统提供的事件名，记录实际输出及工具版本；截图/文字：待填。[Microsoft WPR 命令说明](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/wpr-command-line-options)，查阅 2026-11-02。
- [ ] 可选短时 xperf 探路：仅在实际列出并确认事件、采集配置与权限后进行；不能假定 InstructionRetired / TotalCycles 存在或直接对应进程总计。[Microsoft PMU 事件记录说明](https://learn.microsoft.com/en-us/windows-hardware/test/wpt/recording-pmu-events)，查阅 2026-11-02；采集配置/截图/输出：待填。
- [ ] 如果已安装 VTune，按 Intel 文档从安装目录 bin64 以所需权限运行 `amplxe-sepreg.exe -c` 检查依赖、`amplxe-sepreg.exe -s` 检查驱动状态；输出：待填。[Intel 驱动检查说明](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/2026-0/install-sampling-drivers-for-windows-targets.html)，查阅 2026-11-02。驱动缺失或无法加载时停，不自行安装或改变安全配置。

系统、工具版本、是否可用及选择 Plan A/B：待填。具体 CPU 与 Windows 版本支持情况未核实。
