# 本机环境与验收记录（C0-S01）

- 日期：2026-11-25
- 机器：Windows（版本 10.0.26200.9974），Intel Core Ultra 7 255HX
- 仓库版本：main `c68c86e`

## 工具版本（实测）

- Visual Studio 2026 Community 18.11.1，安装路径 `C:\Program Files\Microsoft Visual Studio\18\Community`
- MSVC：`Microsoft (R) C/C++ Optimizing Compiler Version 19.51.36260 for x64`（vcvars64 初始化为 x64）
- Python 3.13.5；matplotlib 3.10.0
- Sysinternals Coreinfo64：2026-11-25 从微软 Sysinternals 官方包下载，解压在 `%USERPROFILE%\tools\coreinfo\`，用全路径运行（未改 PATH）

## 验收 1：Coreinfo 缓存输出

命令（全路径）：`Coreinfo64.exe -accepteula -c -l`，输出完整跑完。缓存部分摘录（原样）：

```text
Unified Cache       1, Level 3,    30 MB, Assoc 10, LineSize 64
Data Cache          1, Level 1,    48 KB, Assoc 12, LineSize 64
Instruction Cache   1, Level 1,    64 KB, Assoc 16, LineSize 64
Unified Cache       0, Level 2,     3 MB, Assoc 12, LineSize 64
Unified Cache       3, Level 2,     4 MB, Assoc 16, LineSize 64
Data Cache          2, Level 1,    32 KB, Assoc  8, LineSize 64
```

全表规律：L1D 有 48 KB（12 路）与 32 KB（8 路）两种，L1I 均为 64 KB，L2 有 3 MB（12 路）与 4 MB（16 路）两种，L3 为 30 MB，缓存行均为 64。

## 验收 2：cl 编译 blocking.cpp

在 x64 Native Tools 环境、仓库根目录执行：

```bat
cl /nologo /EHsc /O2 /std:c++17 /utf-8 results\src\blocking.cpp /Fe:%TEMP%\c0-s01\blocking.exe /Fo:%TEMP%\c0-s01\blocking.obj
```

输出只有一行 `blocking.cpp`，无警告无报错；产物 `blocking.exe` 为 289,792 字节（2026-11-25）。

注：不加 `/utf-8` 时编译失败——`warning C4819`（当前代码页 936）后 `fatal error C1018: 意外的 #elif`；源码为 UTF-8，本机编译须带 `/utf-8`。

## 验收 3：Python 运行 plot-blocking.py

```bat
python results\scripts\plot-blocking.py --output-dir %TEMP%\c0-s01-plot
```

输出（原样）：

```text
PASS: 75 records, 15 groups, five samples each; no rows excluded.
Wrote summary CSV, SVG and provenance to C:\Users\LEGION\AppData\Local\Temp\c0-s01-plot
```

产物含 `speedup.svg`（43,014 字节）与 `data\summary.csv`。

## 结论

C0-S01 三项验收全部通过，状态：就绪。
