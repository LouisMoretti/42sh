import os
import pytest
import subprocess as sp

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
#       COMMENTS
# ----------------------

def test_execution_cmd_not_found():
    command_to_run = "cmd_not_found"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_expansion_bad_builtin():
    command_to_run = "ech'o 'a b"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, err = proc.communicate(timeout=0.1)
        ref_out, ref_err = ref_proc.communicate(timeout=0.1)
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
