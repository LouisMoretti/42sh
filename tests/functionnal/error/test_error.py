import os
import pytest
import subprocess as sp

def run_ref_command_string(command):
    command = "'" + command + "'"
    proc = sp.Popen(["bash --posix", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    time.sleep(0.1)
    return proc

def run_command_string(command):
    executable = os.getenv["BIN_PATH"]
    if executable is None:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    time.sleep(0.1)
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
