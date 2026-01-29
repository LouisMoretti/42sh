import os
import pytest
import subprocess as sp
import time

def run_ref_command_string(command, args=[]):
    proc = sp.Popen(["bash", "--posix", "-c", command] + args, stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)
    
    return proc

def run_command_string(command, args=[]):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable, "-c", command] + args, stdout=sp.PIPE, stderr=sp.STDOUT, bufsize=0)

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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_while_true():
    command_to_run = "while true; do echo b; echo Hey hey; done"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        _, _ = proc.communicate(timeout=0.5)
        _, _ = ref_proc.communicate(timeout=0.5)
        assert False
    except sp.TimeoutExpired:
        assert True
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_until_false():
    command_to_run = "until false; do echo b; done"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        _, _ = proc.communicate(timeout=0.5)
        _, _ = ref_proc.communicate(timeout=0.5)
        assert False
    except sp.TimeoutExpired:
        assert True
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

def test_until_true_negated():
    command_to_run = "until ! true; do echo b; done"
    proc = run_command_string(command_to_run)
    ref_proc = run_ref_command_string(command_to_run)
    try:
        _, _ = proc.communicate(timeout=0.5)
        _, _ = ref_proc.communicate(timeout=0.5)
        assert False
    except sp.TimeoutExpired:
        assert True
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
    
folder = "medium/tests_files"
param_filepaths = [(open(f"{folder}/{file}", 'r').name, f"{folder}/{file}") for file in os.listdir(folder)]
params_cmds = [("test_mix_medium","if if if false; then echo Fail\nelif false; then echo Aie\nelif true\nthen if echo Hey\nthen echo -n my; else false\nfi\nfi then ls -a; fi\nthen echo little' poney'; fi # So cooked broo", []),
               ("simple_redirection", "echo vivetiger > file.txt", []),
               ("expansion_before_exec", "var=lol echo $var", []),
               ("expansion_in_cmd", "txt=ho; ec$txt good", []),
               ("multiple_expansion", "a=1; b=2; c=3; d=4; echo $a $b $c $d", []),
               ("argument_var_expansion", "echo $0 $1", ['variable0','variable1']),
               ("argument_expansion_twodigits", "echo $11", ['variable0','variable1']),
               ("expansion_bracket","VAR=cletest; echo ${VAR};", []),
               ("expansion_not_found", "echo $skibidi", []),
               ('variable_env_to_cmd', "PWD=src env | grep PWD", []),
               ("simple_while_false", "while false; do echo a; done", []),
               ('simple_until_true', "until true; do echo 42shhhhh; done", []),
               ('simple_for_empty', "for var; do echo -n You are cooooooooked !!!!; done", []),
               ('simple_for_no_expansion', "for v in Hey I am a little poney; do echo -n I am a light sabeeeerrr !; done", []),
               ('simple_for_with_expansion', "for var in My boat sink ...; do echo $var; done", []),
               ('many_or_operators', "false || ! echo Fail || ! echo Aie || echo -n Finally Works !!!", []),
               ('simple_pipe_with_negation', "! echo a | cat", []),
               ('many_boolean_operators', "! echo a || echo b && echo c && ! true || echo gg", []),
               ('pipes_and_or_negation_mixed', "! echo a | cat | cat | false || echo b | cat | cat && echo c && true | false || echo gg | cat | cat | cat", []),
               ('$? in subshell', "echo nope; (echo 0 error); echo $?", []),
               ('recursive_function', "f () { g () { echo Recursive function Yeahhh; }; }; f FAILURE; g;", [])
               ]



@pytest.mark.parametrize("name,command_to_run,list_args", params_cmds)
def test_string(name, command_to_run,list_args):
    proc = run_command_string(command_to_run, list_args)
    ref_proc = run_ref_command_string(command_to_run, list_args)
    try:
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)

@pytest.mark.parametrize("name,command_to_run,list_args", params_cmds)
def test_stdin(name, command_to_run, list_args):
    executable = os.getenv("BIN_PATH")
    if executable is None or len(executable) == 0:
        executable = "../../src/42sh"

    proc = sp.Popen([executable], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)

    ref_proc = sp.Popen(["bash", "--posix"], stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.STDOUT, text=True, bufsize=0)
    try:
        out, err = proc.communicate(input=command_to_run, timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(input=command_to_run, timeout=0.5)
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
        out, err = proc.communicate(timeout=0.5)
        ref_out, ref_err = ref_proc.communicate(timeout=0.5)
        assert out == ref_out
        assert err == ref_err
        assert proc.returncode == ref_proc.returncode
    finally:
        kill_42sh(proc)
        kill_42sh(ref_proc)
