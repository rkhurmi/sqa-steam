# Text between two fields on one line, and the name on the closing line.
SEPARATOR = " "
END_MARKER = "END"

# Characters taken by the cents part of a money text (".00").
DECIMAL_PART_WIDTH = 3

# ---- Fixed-width field sizes of one account line: username type credit ----
USERNAME_WIDTH = 15
USER_TYPE_WIDTH = 2
CREDIT_WIDTH = 9   # e.g. 001000.00

# Position of each field inside a line (computed from the widths above).
USERNAME_START = 0
USER_TYPE_START = USERNAME_START + USERNAME_WIDTH + len(SEPARATOR)
CREDIT_START = USER_TYPE_START + USER_TYPE_WIDTH + len(SEPARATOR)
ACCOUNT_LINE_WIDTH = CREDIT_START + CREDIT_WIDTH   # 28 characters


class AccountsFile:
    """Holds every user account in memory and reads / writes the accounts file."""

    def __init__(self) -> None:
        """Create an empty set of accounts: username -> (user type, credit in cents)."""
        self._users: dict[str, tuple[str, int]] = {}

    def load(self, path: str) -> None:
        """
        Read the current user accounts file into memory.

        Parameters:
            path - path of the accounts file
        Returns nothing. Raises an exception if the file is missing or malformed.
        """
        self._users = {}
        with open(path) as accounts_file:
            for line in accounts_file:
                line = line.rstrip("\n")
                if line.strip() == "":
                    continue
                if line[:USERNAME_WIDTH].rstrip() == END_MARKER:
                    break
                # Cut each fixed-width field out of the line and drop its padding.
                username = line[USERNAME_START:USERNAME_START + USERNAME_WIDTH].rstrip()
                user_type = line[USER_TYPE_START:USER_TYPE_START + USER_TYPE_WIDTH].rstrip()
                credit_text = line[CREDIT_START:CREDIT_START + CREDIT_WIDTH]
                # Money text such as 001000.00 -> 100000 cents.
                dollars_text, cents_text = credit_text.split(".")
                credit_cents = int(dollars_text) * 100 + int(cents_text)
                self._users[username] = (user_type, credit_cents)

    def save(self, path: str) -> None:
        """
        Write every account to a file in the fixed-width format, then the END line.

        Parameters:
            path - path of the file to create
        Returns nothing.
        """
        with open(path, "w") as accounts_file:
            for username, (user_type, credit_cents) in self._users.items():
                # CHECK: money is zero-padded to CREDIT_WIDTH characters; an amount
                # too big for the field would come out too long.
                credit_text = (f"{credit_cents // 100:0{CREDIT_WIDTH - DECIMAL_PART_WIDTH}d}"
                               f".{credit_cents % 100:02d}")
                # CHECK: text fields are left-justified, padded with spaces on the right.
                line = (
                    username.ljust(USERNAME_WIDTH) + SEPARATOR
                    + user_type.ljust(USER_TYPE_WIDTH) + SEPARATOR
                    + credit_text
                )
                accounts_file.write(line + "\n")
            # CHECK: END line. Padded to the full 28 characters here, but the
            # Front End's sample accounts file has a bare "END" - confirm which one.
            accounts_file.write(END_MARKER.ljust(ACCOUNT_LINE_WIDTH) + "\n")

    def exists(self, username: str) -> bool:
        """
        Tell whether an account with this username exists.

        Parameters:
            username - the name to look for
        Returns True if the account exists.
        """
        return username in self._users

    def add_user(self, username: str, type: str, credit_cents: int) -> None:
        """
        Add a new account.

        Parameters:
            username     - name of the new user
            type         - user type code: AA, FS, BS or SS
            credit_cents - starting credit in cents
        Returns nothing.
        """
        self._users[username] = (type, credit_cents)

    def remove_user(self, username: str) -> None:
        """
        Delete an account.

        Parameters:
            username - name of the user to remove
        Returns nothing.
        """
        del self._users[username]

    def get_credit(self, username: str) -> int:
        """
        Look up a user's credit.

        Parameters:
            username - name of the user
        Returns the credit in cents.
        """
        return self._users[username][1]

    def change_credit(self, username: str, amount_cents: int) -> None:
        """
        Add an amount to a user's credit (use a negative amount to subtract).

        Parameters:
            username     - name of the user
            amount_cents - amount to add, in cents
        Returns nothing.
        """
        user_type, credit_cents = self._users[username]
        self._users[username] = (user_type, credit_cents + amount_cents)
