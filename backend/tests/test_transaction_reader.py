import pytest

from error_logger import ErrorLogger
from transaction_reader import TransactionReader, money_to_cents, read_field

# Sample lines in the fixed-width daily transaction file format.
CREATE_LINE = "01 newuser         FS 000050.00"
END_SESSION_LINE = "00                    000000.00"
REFUND_LINE = "05 buyuser         selluser        000010.00"
SELL_LINE = "03 New Game                   selluser        019.99"
BUY_LINE = "04 Stardew Valley             selluser        buyuser         015.00"


class TestReadField:
    def test_sc_rdr_01_cuts_field_and_strips_padding(self):
        """SC-RDR-01: returns the field without its trailing spaces."""
        assert read_field("01 bob             FS", 3, 15) == "bob"


class TestMoneyToCents:
    def test_sc_rdr_02_converts_to_cents(self):
        """SC-RDR-02: dollars and cents text becomes integer cents."""
        assert money_to_cents("001000.50") == 100050


class TestReaderInit:
    def test_sc_rdr_03_stores_logger(self):
        """SC-RDR-03: the reader keeps the logger it is given."""
        logger = ErrorLogger()
        assert TransactionReader(logger)._logger is logger


class TestRead:
    def test_sc_rdr_04_skips_blank_and_end_of_session_lines(self, tmp_path):
        """SC-RDR-04: blank lines and 00 lines are skipped; other lines are kept in order."""
        path = tmp_path / "transactions.txt"
        path.write_text(CREATE_LINE + "\n\n" + END_SESSION_LINE + "\n" + SELL_LINE + "\n")
        transactions = TransactionReader(ErrorLogger()).read(str(path))
        assert [t.code for t in transactions] == ["01", "03"]


class TestParseLine:
    def setup_method(self):
        self.reader = TransactionReader(ErrorLogger())

    def test_sc_rdr_05_create_delete_addcredit_format(self):
        """SC-RDR-05: 01/02/06/00 lines give username, user type and credit."""
        transaction = self.reader.parse_line(CREATE_LINE)
        assert (transaction.code, transaction.username, transaction.user_type,
                transaction.credit_cents) == ("01", "newuser", "FS", 5000)

    def test_sc_rdr_06_refund_format(self):
        """SC-RDR-06: 05 lines give buyer, seller and credit."""
        transaction = self.reader.parse_line(REFUND_LINE)
        assert (transaction.buyer, transaction.seller, transaction.credit_cents) == \
            ("buyuser", "selluser", 1000)

    def test_sc_rdr_07_sell_format(self):
        """SC-RDR-07: 03 lines give game name, seller and price."""
        transaction = self.reader.parse_line(SELL_LINE)
        assert (transaction.game_name, transaction.seller, transaction.price_cents) == \
            ("New Game", "selluser", 1999)

    def test_sc_rdr_08_buy_format(self):
        """SC-RDR-08: 04 lines give game name, seller, buyer and price."""
        transaction = self.reader.parse_line(BUY_LINE)
        assert (transaction.game_name, transaction.seller, transaction.buyer,
                transaction.price_cents) == ("Stardew Valley", "selluser", "buyuser", 1500)

    def test_sc_rdr_09_unknown_code_raises(self):
        """SC-RDR-09: an unknown transaction code raises ValueError."""
        with pytest.raises(ValueError):
            self.reader.parse_line("99 something")
