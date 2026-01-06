import os
import pytest
import subprocess as sp

EXECUTABLE = os.environ["BIN_PATH"] if os.environ["BIN_PATH"] is None else "/"

# ----------------------
#         ECHO
# ----------------------

# ----------------------
#          IF
# ----------------------

def test_if_elif_then_fi:
    pass

def test_many_if:
    pass

def test_classic_if_with_inner_newline:
    pass

# ----------------------
#        BOOLEAN
# ----------------------

# ----------------------
#        QUOTES
# ----------------------

# ----------------------
#       COMMENTS
# ----------------------
def test_echo_quoted_comment:
    pass

def test_echo_quoted_notquoted_comment:
    pass

# ----------------------
#         MIXED
# ----------------------
