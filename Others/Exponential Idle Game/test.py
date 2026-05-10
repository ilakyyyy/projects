import math

def f(x):
    return pow(x, math.pi/math.e)

maxError = 1e10
nMax = 1000000
for i in range(2, nMax+1, 1):
    ceil = False
    actualError = f(i)-math.floor(f(i))
    if actualError > 0.5:
        actualError = math.ceil(f(i))-f(i)
        ceil = True
    if actualError < maxError:
        maxError = actualError
        print(f"New max error: {maxError} at c3={i}, c4 = {math.ceil(f(i)) if ceil else math.floor(f(i))}")
        # if ceil:
        #     print(f"The denominator is >0")
        # else:
        #     print(f"The denominator is <0")