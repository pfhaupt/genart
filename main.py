import subprocess
import os
assert os.name == "nt", f"No support for {os.name} yet"
cc = subprocess.run([
    "clang", "-o", "main.exe", "main.c",
    "-I./raylib/include",
    "-L./raylib/lib/", "-lraylib", "-lOpenGL32", "-lmsvcrt", "-lGdi32", "-lWinMM", "-lkernel32", "-lshell32", "-lUser32",
    "-Xlinker", "/NODEFAULTLIB:libcmt"
], stderr=subprocess.PIPE)
assert cc.returncode == 0, "Could not compile C-Code:\n{}".format(cc.stderr.decode("utf-8"))

gen = subprocess.run(["./main.exe"])
assert gen.returncode == 0, "Could not run main code"