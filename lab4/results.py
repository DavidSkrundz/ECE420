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
result4 = []

for _ in range(10):
	result1.append(float(subprocess.check_output(["mpirun", "-np", "1", "./main"])))
	result4.append(float(subprocess.check_output(["mpirun", "-np", "4", "./main"])))

print "Median Mean"
print "1: {} {}".format(median(result1), mean(result1))
print "4: {} {}".format(median(result4), mean(result4))
