import pytest

from accounts_file import AccountsFile

# Sample lines in the fixed-width accounts file format.
ADMIN_LINE = "admin           AA 001000.00"
BOB_LINE = "bob             FS 000025.50"
END_LINE = "END                000000.00"


def write_file(tmp_path, *lines):
    """Write the given lines to a temporary accounts file and return its path."""
    path = tmp_path / "users.txt"
    path.write_text("".join(line + "\n" for line in lines))
    return str(path)


class TestAccountsInit:
    def test_sc_acc_01_starts_empty(self):
        """SC-ACC-01: a new AccountsFile has no users."""
        assert AccountsFile()._users == {}


class TestAccountsLoad:
    def test_sc_acc_02_loads_users_until_end(self, tmp_path):
        """SC-ACC-02: blank lines are skipped, users are read, reading stops at END."""
        accounts = AccountsFile()
        accounts.load(write_file(tmp_path, ADMIN_LINE, "", BOB_LINE, END_LINE))
        assert accounts._users == {"admin": ("AA", 100000), "bob": ("FS", 2550)}


class TestAccountsSave:
    def test_sc_acc_03_writes_fixed_width_lines_and_end(self, tmp_path):
        """SC-ACC-03: each user is written in fixed-width format, followed by the END line."""
        accounts = AccountsFile()
        accounts.add_user("admin", "AA", 100000)
        path = tmp_path / "new_users.txt"
        accounts.save(str(path))
        assert path.read_text() == ADMIN_LINE + "\n" + "END".ljust(28) + "\n"


class TestAccountsExists:
    def test_sc_acc_04_true_and_false(self):
        """SC-ACC-04: True for a known user, False for an unknown one."""
        accounts = AccountsFile()
        accounts.add_user("bob", "FS", 0)
        assert accounts.exists("bob")
        assert not accounts.exists("alice")


class TestAccountsAddUser:
    def test_sc_acc_05_adds_user(self):
        """SC-ACC-05: the new user is stored with its type and credit."""
        accounts = AccountsFile()
        accounts.add_user("bob", "BS", 500)
        assert accounts._users["bob"] == ("BS", 500)


class TestAccountsRemoveUser:
    def test_sc_acc_06_removes_user(self):
        """SC-ACC-06: the user is gone after removal."""
        accounts = AccountsFile()
        accounts.add_user("bob", "FS", 0)
        accounts.remove_user("bob")
        assert not accounts.exists("bob")


class TestAccountsGetCredit:
    def test_sc_acc_07_returns_credit(self):
        """SC-ACC-07: returns the user's credit in cents."""
        accounts = AccountsFile()
        accounts.add_user("bob", "FS", 1234)
        assert accounts.get_credit("bob") == 1234


class TestAccountsChangeCredit:
    def test_sc_acc_08_adds_amount_and_keeps_type(self):
        """SC-ACC-08: the amount is added to the credit and the user type is unchanged."""
        accounts = AccountsFile()
        accounts.add_user("bob", "FS", 1000)
        accounts.change_credit("bob", -250)
        assert accounts._users["bob"] == ("FS", 750)


# =============================================================================
# PATH COVERAGE - AccountsFile.load
#
# Decisions inside the loop of load():
#   D1: is the line blank?            yes -> continue
#   D2: is the line the END line?     yes -> break
#   otherwise                         -> parse the line and store the user
# The loop can also run zero times, end without an END line, or raise.
# Each test below follows a different path through the method.
# =============================================================================
class TestAccountsFileLoadPathCoverage:
    def load(self, tmp_path, *lines):
        accounts = AccountsFile()
        accounts.load(write_file(tmp_path, *lines))
        return accounts._users

    def test_pc_01_empty_file(self, tmp_path):
        """PC-01: loop runs 0 times."""
        assert self.load(tmp_path) == {}

    def test_pc_02_end_only(self, tmp_path):
        """PC-02: END -> break."""
        assert self.load(tmp_path, END_LINE) == {}

    def test_pc_03_blank_only_no_end(self, tmp_path):
        """PC-03: blank -> continue -> loop ends without END."""
        assert self.load(tmp_path, "") == {}

    def test_pc_04_user_only_no_end(self, tmp_path):
        """PC-04: user -> parse -> loop ends without END."""
        assert self.load(tmp_path, BOB_LINE) == {"bob": ("FS", 2550)}

    def test_pc_05_blank_then_end(self, tmp_path):
        """PC-05: blank -> continue, END -> break."""
        assert self.load(tmp_path, "", END_LINE) == {}

    def test_pc_06_user_then_end(self, tmp_path):
        """PC-06: user -> parse, END -> break."""
        assert self.load(tmp_path, BOB_LINE, END_LINE) == {"bob": ("FS", 2550)}

    def test_pc_07_two_users_then_end_then_ignored_line(self, tmp_path):
        """PC-07: user -> parse, user -> parse, END -> break; lines after END are ignored."""
        users = self.load(tmp_path, ADMIN_LINE, BOB_LINE, END_LINE, "late            FS 000001.00")
        assert users == {"admin": ("AA", 100000), "bob": ("FS", 2550)}

    def test_pc_08_blank_user_end(self, tmp_path):
        """PC-08: blank -> continue, user -> parse, END -> break."""
        assert self.load(tmp_path, "", BOB_LINE, END_LINE) == {"bob": ("FS", 2550)}

    def test_pc_09_missing_file_raises(self, tmp_path):
        """PC-09: open() fails -> exception leaves the method."""
        with pytest.raises(FileNotFoundError):
            AccountsFile().load(str(tmp_path / "missing.txt"))

    def test_pc_10_malformed_credit_raises(self, tmp_path):
        """PC-10: user line with no '.' in the credit -> parse raises ValueError."""
        with pytest.raises(ValueError):
            self.load(tmp_path, "bob             FS 000002550", END_LINE)
