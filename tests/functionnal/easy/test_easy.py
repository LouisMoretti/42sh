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

def test_echo_no_arg():
    command_to_run = "echo"
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

def test_echo_one_arg():
    command_to_run = "echo Helloo"
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

def test_echo_two_args():
    command_to_run = "echo Helloo WORLD"
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

def test_echo_some_args():
    command_to_run = "echo ACUs have some good advices"
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

def test_echo_del_echo():
    command_to_run = "echo Helloo;"
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

def test_three_echo():
    command_to_run = "echo Helloo; echo World; echo !;"
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

def test_echo_just_backslash_n():
    command_to_run = "echo \\\\n"
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

def test_simple_condition():
    command_to_run = "if true; then echo True; fi"
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

def test_loud_condition():
    command_to_run = "if echo Helloo; then echo Worlddd!; fi"
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

def test_if_else_fi():
    command_to_run = "if true; then echo Ouch; else echo HIhaHaHA; fi"
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

def test_true():
    command_to_run = "true"
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

def test_false():
    command_to_run = "false"
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

def test_if_true():
    command_to_run = "if true; then echo Heyy; fi"
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

def test_if_false():
    command_to_run = "if false; then echo AieAieAie; else echo Dragono Succeed; fi"
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
#        QUOTES
# ----------------------

def test_echo_with_single_quote():
    command_to_run = "echo 'Hey ratoons'"
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

def test_echo_single_quote_command():
    command_to_run = "echo 'if true; then echo Failed to parse...; fi'"
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
#       COMMENTS
# ----------------------

def test_simple_comment():
    command_to_run = "echo Hey I want a little poney#poney"
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

folder = "easy/tests_files"
param_filepaths = [(open(f"{folder}/{file}", 'r').name, f"{folder}/{file}") for file in os.listdir(folder)]
params_cmds = [("test_escape","echo -e '\\n'",[]),
               ('simple_negation_false', "! false",[]),
               ('simple_negation_true', "! true",[]),
               ('simple_negation_echo', "! echo a b c",[]),
               ('simple_double_quotes', "echo \"caca\n \\q \\$\t\"\n",[]),
               ('ending_simple_semicol', "echo simplesemicol;",[]),
               ('quote_var_1',"echo '$titi'",[]),
               ('command_var_2', "echo $2", ["val0", "val1", "tesVRAIMENTtropUNoufDEchezOUF"]),
               ("simple_var_bracket", "var=titi; echo titi${var}",[]),
               ("two_variables_twice", "var1=titi; var2=tutu; echo $var1 $var2; echo $var1 $var2",[]),
               ("multiple_defs_on_line", "v1=YA v2=K v3=A; echo $v1; echo $v2; echo $v3", []),
               ("ifs3","echo \"$IFS\"; echo \"$IFS\"; echo \"${IFS}\"",[]),
               ('command_var_1',"echo $1 && echo $1;",["var0","216"]),
               ("hard_var_3","var=\"takebon le 42sh\"; echo $var",[]),
               ("questionmark1","true && echo $?",[]),
               ('ifs2',"echo \"$IFS\"; IFS=newifs; echo \"$IFS\"",[]),
               ('composed_var','i=2; var=$i; echo $var',[]),
               ('sharp','echo $#',[]),
               ('empty_var', 'echo $ppx3',[]),
               ('uid',"echo $UID", []),
               ('simple_var',"VAR=roger_par_en_vadrouille; echo ${VAR}",[]),
               ('equal1',"var==; echo $var",[]),
               ('pwd',"echo $PWD",[]),
               ('dollar@',"echo $@", ["adieu","les","ACUS :("]),
               ('dollar*',"echo $*",["merci", "pour", "tous"]),
               ('for in dollar @',"for i in $@; do echo $i; done;", ["co","u","co","u"]),
               ('simple_echo_expansion_negated', "! echo -e '\t\n\\\\'", []),
               ('simple_echo_many_expansion', "echo -e '\\t\\t\\t\\t\\t\\n\\n\\n\\n\\n\\t\\n\\\\end'", []),
               ('simple_stdout_redirection', 'echo Hello World! > /tmp/easy_file1', []),
               ('simple_stderr_redirection', 'ec Fail! 2> /tmp/easy_file2', []),
               ('simple_stdout_append_redirection', '> /tmp/easy_file3 echo Hello World! >> /tmp/easy_file3', []),
               ('exit inside if rule', "if exit 42; then echo nope; fi;", []),
               ('simple_dollar_arrobaz','echo UC${@}', ["as","ACU","sad"])
               ]

@pytest.mark.parametrize("name,command_to_run, list_args", params_cmds)
def test_string(name, command_to_run, list_args):
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

@pytest.mark.parametrize("name,command_to_run, list_args", params_cmds)
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
