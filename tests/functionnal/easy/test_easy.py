import os
import pytest
import subprocess as sp
import time

def run_ref_command_string(command):
    command = "'" + command + "'"
    proc = sp.Popen(["bash --posix", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    
    time.sleep(0.1)
    return proc

def run_command_string(command):
    executable = os.getenv("BIN_PATH")
    if executable is None:
        executable = "/42sh"
    executable = ". " + executable

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    
    time.sleep(0.1)
    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#         ECHO
# ----------------------
def test_echo_no_arg():
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

def test_echo_one_arg():
    command_to_run = "echo Helloo"
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

def test_echo_two_args():
    command_to_run = "echo Helloo WORLD"
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
    

def test_echo_some_args():
    command_to_run = "echo ACUs have some good advices"
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
    

def test_echo_del_echo():
    command_to_run = "echo Helloo;"
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
    

def test_three_echo():
    command_to_run = "echo Helloo; echo World; echo !;"
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
    

def test_echo_newline():
    command_to_run = "echo \\n"
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
    

def test_echo_arg_with_newline():
    command_to_run = "echo Helloo WoRld\\n\\n"
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
#          IF
# ----------------------
def test_simple_condition():
    command_to_run = "if 1=1 then echo True fi"
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
    

def test_loud_condition():
    command_to_run = "if echo Helloo then echo Worlddd! fi"
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
    

def test_if_else_fi():
    command_to_run = "if 42=21 then echo Ouch else HIhaHaHA fi"
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
#        BOOLEAN
# ----------------------
def test_true():
    command_to_run = "true"
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

def test_false():
    command_to_run = "false"
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

def test_if_true():
    command_to_run = "if true then echo Heyy fi"
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
    
def test_if_false():
    command_to_run = "if false then echo AieAieAie else echo Dragono Succeed fi"
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
#        QUOTES
# ----------------------

def test_echo_with_single_quote():
    command_to_run = "echo 'Hey ratoons'"
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

def test_echo_single_quote_command():
    command_to_run = "echo 'if true then echo Failed to parse... fi'"
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
#       COMMENTS
# ----------------------

def test_simple_comment():
    command_to_run = "echo Hey I want a little #poney"
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
