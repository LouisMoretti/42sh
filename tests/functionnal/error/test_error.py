import os
import pytest
import subprocess as sp

def run_ref_command_string(command):
    proc = sp.Popen(["bash --posix", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def run_command_string(command):
    executable = os.getenv["BIN_PATH"]
    if executable is None:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#       COMMENTS
# ----------------------

def test_example():
    command_to_run = "echo"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command(command_to_run)
    try:
        out, err = proc.communicate(timeout=1)
        ref_out, ref_err = ref_proc.communicate(timeout=1)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

folder = "error/tests_files"
param_filepaths = [(open(f"{folder}/{file}", 'r').name, f"{folder}/{file}") for file in os.listdir(folder)]
params_cmds = [("bad expansion", "var3=lol; var2=var3; echo ${$var2}")]

@pytest.mark.parametrize("name,command_to_run", params_cmds)
def test_string(name, command_to_run):
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        _ , err = proc.communicate(timeout=0.1)
        assert err != None
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

@pytest.mark.parametrize("name,command_to_run", params_cmds)
def test_stdin(name, command_to_run):
    executable = os.getenv("BIN_PATH")
    if executable is None:
        executable = "../../src/42sh"

    proc = sp.Popen([executable], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)

    ref_proc = sp.Popen(["bash", "--posix"], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)
    try:
        _ , err = proc.communicate(input=command_to_run, timeout=0.1)
        assert err != None
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

@pytest.mark.parametrize("name,filepath", param_filepaths)
def test_file(name, filepath):
    executable = os.getenv("BIN_PATH")
    if executable is None:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, filepath], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    ref_proc = sp.Popen(["bash", "--posix", filepath], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    try:
        _ , err = proc.communicate(timeout=0.1)
        assert err != None
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
