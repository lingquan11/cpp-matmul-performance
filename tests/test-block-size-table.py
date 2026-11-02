# AI 维护：公式和比较的最小测试，不使用实测数据。
import importlib.util
from pathlib import Path
spec = importlib.util.spec_from_file_location("block_size_table", Path(__file__).resolve().parents[1] / "scripts/block-size-table.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
assert module.footprint(32) == 12288
assert module.footprint(16) < module.footprint(32)
print("PASS: footprint formula")
