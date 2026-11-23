# AI 维护：人工构造临时 fixture，非实测。
import subprocess, sys, tempfile
from pathlib import Path
script=Path(__file__).resolve().parents[1]/"scripts/summarize-gemm-shapes.py"
with tempfile.TemporaryDirectory() as tmp:
    root=Path(tmp)
    for run in range(1,11):
        for i,shape in enumerate(("square","tall","wide","deep","shortk"),1):
            d=root/f"run{run:02d}-{shape}"; d.mkdir()
            (d/"sample.csv").write_text(f"gmac_per_s\n{i}\n",encoding="utf-8")
    result=subprocess.run([sys.executable,str(script),str(root)],capture_output=True,text=True,check=True)
    assert "shape sensitivity established" in result.stdout
print("PASS: synthetic summary fixture (not measured)")
