import os
import pytest
import subprocess as sp
import time

def run_command_string(command):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "--pretty-print", "-c", command], stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

    return proc

def kill_42sh(proc):
    proc.kill()

# ----------------------
#          IF
# ----------------------

def test_pretty_if():
    command_to_run = "if true; then echo coucou; elif false; then echo oui; else echo non; fi;"
    proc = run_command_string(command_to_run)
    try:
        out, _ = proc.communicate(timeout=0.1)
        ref_out = "if true; then\n    echo coucou;\nelif false; then\n    echo oui;\nelse\n    echo non;\nfi;\n\n".encode()
        assert out == ref_out
    finally:
        kill_42sh(proc)

# ----------------------
#         WHILE
# ----------------------

def test_pretty_while():
    command_to_run = "while echo oui; do echo non; done"
    proc = run_command_string(command_to_run)
    try:
        out, _ = proc.communicate(timeout=0.1)
        ref_out = "".encode()
        assert out == ref_out
    finally:
        kill_42sh(proc)
