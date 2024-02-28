import subprocess

subprocess.run(["gcovr", "--html-details", "-o", "./test/coverage/main.html"])
print ("\033[93;1;4mCoverage Report Built\033[0m")

