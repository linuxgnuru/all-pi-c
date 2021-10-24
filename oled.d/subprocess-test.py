import subprocess

p = subprocess.Popen("date", stdout=subprocessPIPE, shell=True)
(output, err) = p.communicate()
print "Today is ", output

p = subprocess.Popen(["ls", "-l", "."], stdout=subprocessPIPE)
(output, err) = p.communicate()
print "Result ", output
