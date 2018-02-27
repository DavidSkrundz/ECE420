import subprocess

def mean(numbers):
	return float(sum(numbers)) / max(len(numbers), 1)

def median(lst):
	n = len(lst)
	if n < 1:
		return None
	if n % 2 == 1:
		return sorted(lst)[n//2]
	else:
		return sum(sorted(lst)[n//2-1:n//2+1])/2.0

result1 = []
result2 = []
result4 = []
result8 = []

for _ in range(100):
	result1.append(float(subprocess.check_output(["./main", "1"])))
	result2.append(float(subprocess.check_output(["./main", "2"])))
	result4.append(float(subprocess.check_output(["./main", "4"])))
	result8.append(float(subprocess.check_output(["./main", "8"])))

print "Median Mean"
print "1: {} {}".format(median(result1), mean(result1))
print "2: {} {}".format(median(result2), mean(result2))
print "4: {} {}".format(median(result4), mean(result4))
print "8: {} {}".format(median(result8), mean(result8))
