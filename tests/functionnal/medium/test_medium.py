import os
import pytest
import subprocess as sp
import time

def run_ref_command_string(command):
    proc = sp.Popen(["bash", "--posix", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def run_command_string(command):
    executable = os.getenv("BIN_PATH")
    if executable is None:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#         ECHO
# ----------------------

def test_echo_arg_with_newline():
    command_to_run = "echo -e Helloo\\\\nWoRld\\\\n!"
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

def test_echo_arg_with_skip_and_newline():
    command_to_run = "echo -e Helloo WoRld a\\nd bye bye \\\\n hihi !"
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

def test_if_elif_then_fi():
    command_to_run = "if false; then echo Ouch; elif true; then echo Bravo; fi"
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

def test_classic_if_with_inner_newline():
    command_to_run = "if \n echo -e hello there !; \n then \n echo gg bro; \n fi"
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

# ----------------------
#        QUOTES
# ----------------------

# ----------------------
#       COMMENTS
# ----------------------

def test_echo_quoted_comment():
    command_to_run = "echo this comment is \\#escape"
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

def test_echo_quoted_notquoted_comment():
    command_to_run = "echo \\#escaped \'#\'quoted not#first #commented"
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
#         PIPE
# ----------------------

def test_simple_echo_pipe():
    command_to_run = "echo a | echo b"
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

def test_simple_echo_and_many_cat():
    command_to_run = "echo SUCCEED | cat | cat | cat | cat | cat | cat | cat"
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

def test_simple_several_commands_and_pipes():
    command_to_run = "echo Hey | cat | cat; echo my | cat | cat; echo little | cat | cat; echo poney | cat | cat | cat"
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
#         MIXED
# ----------------------

def test_echo_backslash_end_comment():
    command_to_run = "echo \\\\Hello There #Fuck you bro"
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

def test_simple_several_commands_and_ands_pipes():
    command_to_run = "echo Hey | cat | cat && echo my | cat | cat && false || echo little | cat | cat && echo poney | cat | cat | cat"
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
