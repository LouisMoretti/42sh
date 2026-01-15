import os
import pytest
import subprocess as sp
import time

def run_ref_command_string(command):
    proc = sp.Popen(["bash", "--pretty-print", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def run_command_string(command):
    executable = os.getenv("BIN_PATH")
    if executable is None:
        executable = "../../../src/42sh"

    proc = sp.Popen([executable, "-c", "--pretty-print", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#          IF
# ----------------------

def test_pretty_if():
    command_to_run = "if true; then echo H; fi; if true; then echo e; fi; if true; then echo e; fi; if false; then echo Aie; else echo ll; fi; if true; then echo o; else echo ooo; fi; if true; then if false; then echo Hi; else echo ' World'; fi fi; if false; then false; elif false; then false; elif false; then false; else true; fi;"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        out, _ = proc.communicate(timeout=1)
        ref_out, _ = ref_proc.communicate(timeout=1)
        assert out == ref_out
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
