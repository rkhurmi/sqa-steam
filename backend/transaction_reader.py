from error_logger import ErrorLogger
from transaction import Transaction

# Text placed between two fields on one line (a single space).
SEPARATOR = " "

# ---- Transaction codes ----
CODE_CREATE = "01"
CODE_DELETE = "02"
CODE_SELL = "03"
CODE_BUY = "04"
CODE_REFUND = "05"
CODE_ADD_CREDIT = "06"
CODE_END_OF_SESSION = "00"

# ---- Fixed-width field sizes of the daily transaction file ----
CODE_WIDTH = 2
USERNAME_WIDTH = 15
USER_TYPE_WIDTH = 2
CREDIT_WIDTH = 9       # e.g. 001000.00
GAME_NAME_WIDTH = 26
PRICE_WIDTH = 6        # e.g. 015.00

# Fields start after the code and one separator.
FIRST_FIELD_START = CODE_WIDTH + len(SEPARATOR)


def read_field(line: str, start: int, width: int) -> str:
    """
    Cut one fixed-width field out of a line.

    Parameters:
        line  - the full text line
        start - index of the first character of the field
        width - number of characters in the field
    Returns the field with its trailing padding spaces removed.
    """
    return line[start:start + width].rstrip()


def money_to_cents(money_text: str) -> int:
    """
    Convert money text such as "001000.00" into integer cents (100000).

    Parameters:
        money_text - dollars and cents separated by a dot
    Returns the amount in cents.
    """
    dollars_text, cents_text = money_text.split(".")
    return int(dollars_text) * 100 + int(cents_text)


class TransactionReader:
    """Reads the merged daily transaction file and turns each line into a Transaction."""

    def __init__(self, logger: ErrorLogger) -> None:
        """
        Create the reader.

        Parameters:
            logger - the shared ErrorLogger
        """
        self._logger: ErrorLogger = logger

    def read(self, path: str) -> list[Transaction]:
        """
        Read the merged transaction file.

        Parameters:
            path - path of the merged transaction file
        Returns the transactions in file order. End-of-session (00) lines are
        skipped. Raises an exception if the file is missing or a line is bad.
        """
        transactions: list[Transaction] = []
        with open(path) as transaction_file:
            for line in transaction_file:
                line = line.rstrip("\n")
                if line.strip() == "":
                    continue
                transaction = self.parse_line(line)
                if transaction.code == CODE_END_OF_SESSION:
                    continue
                transactions.append(transaction)
        return transactions

    def parse_line(self, line: str) -> Transaction:
        """
        Turn one text line into a Transaction. Fields that the transaction
        code does not use stay None.

        Parameters:
            line - one line of the transaction file
        Returns the Transaction. Raises ValueError for an unknown code.
        """
        code = read_field(line, 0, CODE_WIDTH)
        position = FIRST_FIELD_START

        if code in (CODE_CREATE, CODE_DELETE, CODE_ADD_CREDIT, CODE_END_OF_SESSION):
            # CC UUUUUUUUUUUUUUU TT CCCCCCCCC
            username = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            user_type = read_field(line, position, USER_TYPE_WIDTH)
            position += USER_TYPE_WIDTH + len(SEPARATOR)
            credit_cents = money_to_cents(read_field(line, position, CREDIT_WIDTH))
            return Transaction(code, username=username, user_type=user_type,
                               credit_cents=credit_cents)

        if code == CODE_REFUND:
            # 05 buyer seller credit
            buyer = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            seller = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            credit_cents = money_to_cents(read_field(line, position, CREDIT_WIDTH))
            return Transaction(code, buyer=buyer, seller=seller, credit_cents=credit_cents)

        if code == CODE_SELL:
            # 03 game seller price
            game_name = read_field(line, position, GAME_NAME_WIDTH)
            position += GAME_NAME_WIDTH + len(SEPARATOR)
            seller = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            price_cents = money_to_cents(read_field(line, position, PRICE_WIDTH))
            return Transaction(code, game_name=game_name, seller=seller, price_cents=price_cents)

        if code == CODE_BUY:
            # 04 game seller buyer price
            game_name = read_field(line, position, GAME_NAME_WIDTH)
            position += GAME_NAME_WIDTH + len(SEPARATOR)
            seller = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            buyer = read_field(line, position, USERNAME_WIDTH)
            position += USERNAME_WIDTH + len(SEPARATOR)
            price_cents = money_to_cents(read_field(line, position, PRICE_WIDTH))
            return Transaction(code, game_name=game_name, seller=seller, buyer=buyer,
                               price_cents=price_cents)

        raise ValueError(f"unknown transaction code '{code}'")
