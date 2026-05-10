def factorial(n):
    if n == 0 or n == 1:
        return 1
    else:
        return n * factorial(n - 1)

def sum_factorials(n):
    total = 0
    for i in range(n + 1):
        total += factorial(n)/factorial(i)
    return total

num = 0
den = 0
nMax = 10
for i in range(nMax):
    num = sum_factorials(i)
    den = factorial(i)
    print(f"n={i}, num={num}, den={den}, ratio={num/den}")