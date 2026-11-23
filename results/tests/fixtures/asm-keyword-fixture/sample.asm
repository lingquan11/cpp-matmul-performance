; fixture
; 手写夹具，不是编译器输出，也不是实测。不要把这里的指令计数写进报告。
; 注释里提到 vpmulld 和 pmulld，计数必须忽略这一行。

matmul_ikj PROC
    vpmulld ymm0, ymm1, ymm2
    pmulld xmm0, xmm1
matmul_ikj ENDP

matmul_blocked PROC
    pmulld xmm2, xmm3
    pmulld xmm4, xmm5
matmul_blocked ENDP

other_helper PROC
    nop
other_helper ENDP
