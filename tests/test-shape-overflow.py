# AI 维护：用小矩阵精确点积核验保守上界，并测试非法输入。
import importlib.util
from pathlib import Path
spec=importlib.util.spec_from_file_location("overflow",Path(__file__).resolve().parents[1]/"scripts/check-shape-overflow.py")
m=importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
for M,K,N in [(3,5,7),(1,16,1)]:
    exact=sum((M-1+k+1)*(2*k+N-1+1) for k in range(K))
    assert exact <= m.bound(M,K,N)
try: m.bound(0,1,1); raise AssertionError("accepted zero")
except ValueError: pass
print("PASS: overflow bounds")
