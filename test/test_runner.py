import os
import re
from subprocess import Popen, PIPE, DEVNULL

import sys

compiler_command = "/home/aritz/Documentos/Proyectos/C++/FleetLang/cmake-build-debug/FleetLang - -o "
linking_command = "clang++ test.cpp {} -o {} -v"


def compile(code, out_file="/dev/null"):
    process = Popen(compiler_command + out_file, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    out, err = process.communicate(input=code.encode('utf-8'))
    return process.returncode, out.decode("utf-8"), err.decode("utf-8")


def run_obj(file):
    linker = Popen(linking_command.format(file, "output"), shell=True, stdout=DEVNULL, stderr=DEVNULL)
    out, err = linker.communicate()
    if linker.returncode != 0:
        return False, out.decode('utf-8'), err.decode('utf-8')

    command = Popen("./output", shell=True, stdout=PIPE, stderr=PIPE)
    out, err = command.communicate()
    return command.returncode, out.decode('utf-8'), err.decode('utf-8')


abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

print("Running tests...")

total_tests = 0
passed_tests = 0

for filename in os.listdir('tests'):
    if filename.endswith(".out"):
        continue
    total_tests += 1

    with open('tests/' + filename, 'r') as myfile:
        data = myfile.read()

    _, first, program = re.split('^([^\n]+)\n', data)
    expected = False if re.match("//\s+(\w+)", first).group(1) == "FAIL" else True
    without_extension = re.match("([^.]*)\.[^.]*", filename).group(1)

    if expected:
        retcode, out, err = compile(program, "test.o")
        if retcode == 0:
            # Run the program, check the output
            retcode, stout, sterr = run_obj("test.o")
            with open('tests/' + without_extension + ".out", 'r') as myfile:
                expected_output = myfile.read()

            if expected_output != stout:
                print("Error in test '" + filename + "'", file=sys.stderr)
                print(err, file=sys.stderr)
            else:
                passed_tests += 1
    else:
        retcode, out, err = compile(program)
        if retcode != 0:
            passed_tests += 1
        else:
            print("Error in test '" + filename + "'", file=sys.stderr)
            print(err, file=sys.stderr)


def remove_file(name):
    if os.path.isfile(name):
        os.remove(name)


remove_file("output")
remove_file("output.o")
remove_file("test")
remove_file("test.o")

print("Passed {} out of {} tests!".format(passed_tests, total_tests))
if passed_tests != total_tests:
    exit(1)
else:
    exit(0)