# AI 维护：仅核对手算，不是硬件读取或测量。
# python scripts/block-size-table.py --l1-bytes L1 --l2-bytes L2
import argparse

def footprint(b):
    return 3 * b * b * 4

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--l1-bytes", required=True, type=int)
    parser.add_argument("--l2-bytes", required=True, type=int)
    args = parser.parse_args()
    if args.l1_bytes <= 0 or args.l2_bytes <= 0: parser.error("capacities must be positive")
    print("b,F_bytes,fits_L1D,fits_L2")
    for b in (16, 32, 64, 128, 256):
        f = footprint(b)
        print(f"{b},{f},{f <= args.l1_bytes},{f <= args.l2_bytes}")
if __name__ == "__main__": main()
