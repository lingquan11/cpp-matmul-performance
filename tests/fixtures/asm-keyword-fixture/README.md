# 汇编关键字夹具

格式测试用，非实测。

`sample.asm` 和 `sample-vec.txt` 是手写的，不是 `cl /FAs` 或 `/Qvec-report:2` 的输出。里面的指令和原因码只用来检查脚本会不会把 `vpmulld` 再算进 `pmulld`，以及会不会拆行。不能写进正式报告，也不能当成我这台电脑的向量化结论。
