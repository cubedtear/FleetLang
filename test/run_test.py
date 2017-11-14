#!/usr/bin/env python3
import os
import re
import sys
from subprocess import Popen, PIPE


def remove_file(name):
    if os.path.isfile(name):
        os.remove(name)


def clean():
    remove_file("output")
    remove_file("output.o")
    remove_file("test")
    remove_file("test.o")


compiler_path = sys.argv[1]
input_file = sys.argv[2]

compiler_command = "{} - -o ".format(compiler_path)
linking_command = "g++ {}/test.cpp {} -o {} -v".format(os.path.dirname(os.path.realpath(__file__)), "{}", "{}")


def compile(code, out_file="/dev/null"):
    process = Popen(compiler_command + out_file, shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    out, err = process.communicate(input=code.encode('utf-8'))
    return process.returncode, out.decode("utf-8"), err.decode("utf-8")


def run_obj(file):
    linker = Popen(linking_command.format(file, "output"), shell=True, stdout=PIPE, stderr=PIPE)
    out, err = linker.communicate()
    if linker.returncode != 0:
        return False, out.decode('utf-8'), err.decode('utf-8')

    command = Popen("./output", shell=True, stdout=PIPE, stderr=PIPE)
    out, err = command.communicate()
    return command.returncode, out.decode('utf-8'), err.decode('utf-8')


with open(input_file, 'r') as myfile:
    data = myfile.read()

_, first, program = re.split('^([^\n]+)\n', data)
expected = False if re.match("//\s+(\w+)", first).group(1) == "FAIL" else True

without_extension = re.match("(.*)\.[^.]*", input_file).group(1)

if expected:
    retcode, out, err = compile(program, "test.o")
    if retcode == 0:
        # Run the program, check the output
        retcode, stout, sterr = run_obj("test.o")
        output_file_path = os.path.join(os.path.dirname(input_file), without_extension + ".out")
        with open(output_file_path, 'r') as myfile:
            expected_output = myfile.read()
        clean()
        if expected_output != stout:
            print("Error in test '" + input_file + "'", file=sys.stderr)
            print(out, file=sys.stderr)
            print(err, file=sys.stderr)
            exit(-1)
        else:
            exit(0)
else:
    retcode, out, err = compile(program)
    clean()
    if retcode != 0:
        exit(0)
    else:
        exit(-1)
