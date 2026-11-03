# AI 维护：Day 7 公式推广的保守 int32 上界，输入方案待确认，不是硬件测量。
# python scripts/check-shape-overflow.py
SHAPES = [(256,256,256),(2048,64,128),(32,512,1024),(256,1024,64),(1024,16,1024)]
def bound(m,k,n):
    if min(m,k,n)<=0: raise ValueError("dimensions must be positive")
    return k*(m+k-1)*(2*k+n-2)
if __name__=="__main__":
    for shape in SHAPES:
        value=bound(*shape)
        print(*shape, "upper_bound",value,"fits_int32",value<=2**31-1)
