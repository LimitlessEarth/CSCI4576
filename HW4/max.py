import sys
for line in sys.stdin:
    max = 0.0
    nums =  line.split(" ")
    nums.pop()
    for num in nums:
        if float(num) > max:
            max = float(num)
    print max
