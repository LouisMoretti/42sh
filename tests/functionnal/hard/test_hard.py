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
        executable = "/42sh"
    executable = ". " + executable

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    
    time.sleep(0.1)
    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#          IF
# ----------------------

def test_many_if():
    command_to_run = "if true; then echo H; fi; if true; then echo e; fi; if true; then echo e; fi; if false; then echo Aie; else echo ll; fi; if true; then echo o; else echo ooo; fi; if true; then if false; then echo Hi; else echo ' World'; fi fi; if false; then false; elif false; then false; elif false; then false; else true; fi;"
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



# ----------------------
#         MIXED
# ----------------------

def test_mixed_demon():
    command_to_run = "if true; then echo -n G; fi; if true; then echo -n G; echo -n ' '; fi; if false; then echo Aie; else echo !; fi; if echo -n You; then echo -n ' '; else echo booooo; fi; if true; then if false; then echo noobs; else echo -n are; fi fi; if false; then false; elif echo -n ' soooo '; then echo strong !; else true; fi; echo -E that is an \# btw, not the next one here : # stupid human;"
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


