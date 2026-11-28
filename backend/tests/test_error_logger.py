import pytest

from error_logger import ErrorLogger
from transaction import Transaction


class TestConstraintError:
    def test_sc_log_01_prints_message(self, capsys):
        """SC-LOG-01: message is printed in the ERROR format and the program keeps going."""
        ErrorLogger().constraint_error("DuplicateUsername", "user bob already exists",
                                       Transaction("01"))
        assert capsys.readouterr().out == \
            "ERROR: DuplicateUsername: user bob already exists (transaction code 01)\n"


class TestFatalError:
    def test_sc_log_02_prints_and_exits(self, capsys):
        """SC-LOG-02: message is printed and the program exits with code 1."""
        with pytest.raises(SystemExit) as exit_info:
            ErrorLogger().fatal_error("BadFile", "missing", "users.txt")
        assert exit_info.value.code == 1
        assert capsys.readouterr().out == "ERROR: BadFile: missing (file users.txt)\n"
