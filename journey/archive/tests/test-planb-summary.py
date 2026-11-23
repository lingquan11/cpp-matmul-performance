# AI 维护：人工生成临时 fixture，仅测试格式，非实测。
import os,subprocess,sys,tempfile
from pathlib import Path
script=Path(__file__).resolve().parents[1]/"scripts/summarize-planb.py"
with tempfile.TemporaryDirectory() as tmp:
    for i in range(1,11):
        for variant,value in (("real",2),("resident",1)):
            d=Path(tmp)/f"n64-run{i:02d}-{variant}"; d.mkdir()
            (d/"sample.csv").write_text(f"algo,n,variant,repeats,ms_per_multiply,checksum\nikj,64,{variant},1,{value},0\n")
    p=subprocess.run([sys.executable,str(script),tmp],capture_output=True,text=True,check=True,encoding="utf-8",env={**os.environ,"PYTHONIOENCODING":"utf-8"})
    assert "median_delta 0.5" in p.stdout and "same side, >=5%" in p.stdout
print("PASS: synthetic paired fixture (not measured)")
