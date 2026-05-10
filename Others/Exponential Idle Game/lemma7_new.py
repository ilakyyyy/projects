# def generate_array(n):
#     array = [2,1,2]
#     for i in range(n):
#         for j in range(i):
#             array.append(1) 
#         array.append(2*(i+1))
#     if n > 3:
#         array[3] = 1
#     return(array)

# def approximation(array_limit):
#     array = generate_array(array_limit)
    
#     num = array[-1]
#     den = 1
    
#     for x in array[:-1][::-1]:
        
#         new_num = x * num + den
#         new_den = num
        
#         num = new_num
#         den = new_den
        
#     return num, den

# nIterations = 10
# for i in range(nIterations):
#     approx = approximation(i)
#     print(f"i={i}, e={approx}; {approx[0]/approx[1]}")

def get_sequence(limit):
    seq = [2]
    for k in range(1, limit + 1):
        seq.extend([1, 2 * k, 1])
    return seq[:limit]

def compute_fraction(seq):
    num = seq[-1]
    den = 1

    for x in seq[:-1][::-1]:
        num, den = x * num + den, num
        
    return num, den

full_pattern = get_sequence(20)
print(f"Full Pattern: {full_pattern}")
print("-" * 50)

for i in range(len(full_pattern)):
    current_slice = full_pattern[:i+1]
    
    num, den = compute_fraction(current_slice)
    val = num / den
    
    print(f"Terms: {i+1} | Seq: {current_slice} | Fraction: {num}/{den} | Value: {val:.5f}")
