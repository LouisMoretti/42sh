import os
import pytest
import subprocess as sp
import time

def run_ref_command_string(command):
    proc = sp.Popen(["bash", "--posix", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def run_command_string(command):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#         ECHO
# ----------------------

def test_echo_no_arg():
    command_to_run = "echo"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_one_arg():
    command_to_run = "echo Helloo"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_two_args():
    command_to_run = "echo Helloo WORLD"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_some_args():
    command_to_run = "echo ACUs have some good advices"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_del_echo():
    command_to_run = "echo Helloo;"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_three_echo():
    command_to_run = "echo Helloo; echo World; echo !;"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_just_backslash_n():
    command_to_run = "echo \\\\n"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

# ----------------------
#          IF
# ----------------------

def test_simple_condition():
    command_to_run = "if true; then echo True; fi"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_loud_condition():
    command_to_run = "if echo Helloo; then echo Worlddd!; fi"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_if_else_fi():
    command_to_run = "if true; then echo Ouch; else echo HIhaHaHA; fi"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

# ----------------------
#        BOOLEAN
# ----------------------

def test_true():
    command_to_run = "true"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_false():
    command_to_run = "false"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_if_true():
    command_to_run = "if true; then echo Heyy; fi"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_if_false():
    command_to_run = "if false; then echo AieAieAie; else echo Dragono Succeed; fi"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)


# ----------------------
#        QUOTES
# ----------------------

def test_echo_with_single_quote():
    command_to_run = "echo 'Hey ratoons'"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_echo_single_quote_command():
    command_to_run = "echo 'if true; then echo Failed to parse...; fi'"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

# ----------------------
#       COMMENTS
# ----------------------

def test_simple_comment():
    command_to_run = "echo Hey I want a little poney#poney"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

folder = "easy/tests_files"
param_filepaths = [(open(f"{folder}/{file}", 'r').name, f"{folder}/{file}") for file in os.listdir(folder)]
params_cmds = [("test_escape","echo -e '\\n'"),
               ('simple_negation_false', "! false"),
               ('simple_negation_true', "! true"),
               ('simple_negation_echo', "! echo a b c"),
               ('ending_simple_semicol', "echo simplesemicol;")
               ]

@pytest.mark.parametrize("name,command_to_run", params_cmds)
def test_string(name, command_to_run):
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

@pytest.mark.parametrize("name,command_to_run", params_cmds)
def test_stdin(name, command_to_run):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)

    ref_proc = sp.Popen(["bash", "--posix"], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)
    try:
        out, err = proc.communicate(input=command_to_run, timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(input=command_to_run, timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

@pytest.mark.parametrize("name,filepath", param_filepaths)
def test_file(name, filepath):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, filepath], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    ref_proc = sp.Popen(["bash", "--posix", filepath], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
