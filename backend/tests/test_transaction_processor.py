import pytest

from error_logger import ErrorLogger
from transaction import Transaction
from transaction_processor import TransactionProcessor


@pytest.fixture
def processor():
    """A processor with two users, one game for sale and one owned game."""
    processor = TransactionProcessor(ErrorLogger())
    processor._accounts.add_user("buyuser", "BS", 5000)       # $50.00
    processor._accounts.add_user("selluser", "SS", 1000)      # $10.00
    processor._available_games.add_game("Hades", "selluser", 2500)
    processor._collection.add_ownership("Portal 2", "buyuser")
    return processor


class TestProcessorInit:
    def test_sc_prc_01_starts_empty(self):
        """SC-PRC-01: a new processor has empty data and keeps the logger."""
        logger = ErrorLogger()
        processor = TransactionProcessor(logger)
        assert processor._accounts._users == {}
        assert processor._available_games._games == {}
        assert processor._collection._entries == set()
        assert processor._logger is logger


class TestApply:
    @pytest.mark.parametrize("code, handler", [
        ("01", "_create"), ("02", "_delete"), ("03", "_sell"),
        ("04", "_buy"), ("05", "_refund"), ("06", "_add_credit"),
    ])
    def test_sc_prc_02_calls_matching_handler(self, processor, monkeypatch, code, handler):
        """SC-PRC-02: each code calls its own handler."""
        called = []
        monkeypatch.setattr(processor, handler, lambda transaction: called.append(transaction))
        transaction = Transaction(code)
        processor.apply(transaction)
        assert called == [transaction]

    def test_sc_prc_03_unknown_code_does_nothing(self, processor):
        """SC-PRC-03: a code with no handler changes nothing."""
        processor.apply(Transaction("00"))
        assert processor._accounts.get_credit("buyuser") == 5000


class TestCreate:
    def test_sc_prc_04_adds_new_user(self, processor):
        """SC-PRC-04: a new username is added."""
        processor._create(Transaction("01", username="newuser", user_type="FS", credit_cents=0))
        assert processor._accounts.exists("newuser")

    def test_sc_prc_05_duplicate_username_rejected(self, processor, capsys):
        """SC-PRC-05: an existing username logs DuplicateUsername and is not changed."""
        processor._create(Transaction("01", username="buyuser", user_type="FS", credit_cents=0))
        assert "DuplicateUsername" in capsys.readouterr().out
        assert processor._accounts._users["buyuser"] == ("BS", 5000)


class TestDelete:
    def test_sc_prc_06_removes_user_games_and_collection(self, processor):
        """SC-PRC-06: the user, their games for sale and their owned games are removed."""
        processor._delete(Transaction("02", username="selluser"))
        processor._delete(Transaction("02", username="buyuser"))
        assert not processor._accounts.exists("selluser")
        assert not processor._available_games.exists("Hades")
        assert not processor._collection.owns("buyuser", "Portal 2")

    def test_sc_prc_07_unknown_user_rejected(self, processor, capsys):
        """SC-PRC-07: deleting a user that does not exist logs an error instead of crashing."""
        processor._delete(Transaction("02", username="ghost"))
        assert "UnknownUser" in capsys.readouterr().out


class TestSell:
    def test_sc_prc_08_lists_game(self, processor):
        """SC-PRC-08: the game is added to the available games."""
        processor._sell(Transaction("03", game_name="Celeste", seller="selluser",
                                    price_cents=1999))
        assert processor._available_games._games["Celeste"] == ("selluser", 1999)


class TestBuy:
    def test_sc_prc_09_successful_buy(self, processor):
        """SC-PRC-09: credit moves buyer -> seller and the buyer owns the game."""
        processor._buy(Transaction("04", game_name="Hades", seller="selluser",
                                   buyer="buyuser", price_cents=2500))
        assert processor._accounts.get_credit("buyuser") == 2500
        assert processor._accounts.get_credit("selluser") == 3500
        assert processor._collection.owns("buyuser", "Hades")

    def test_sc_prc_10_already_owned_rejected(self, processor, capsys):
        """SC-PRC-10: buying a game the buyer already owns logs AlreadyOwned."""
        processor._buy(Transaction("04", game_name="Portal 2", seller="selluser",
                                   buyer="buyuser", price_cents=100))
        assert "AlreadyOwned" in capsys.readouterr().out
        assert processor._accounts.get_credit("buyuser") == 5000

    def test_sc_prc_11_insufficient_credit_rejected(self, processor, capsys):
        """SC-PRC-11: a buyer who cannot afford the game logs InsufficientCredit."""
        processor._buy(Transaction("04", game_name="Hades", seller="buyuser",
                                   buyer="selluser", price_cents=2500))
        assert "InsufficientCredit" in capsys.readouterr().out
        assert not processor._collection.owns("selluser", "Hades")


class TestRefund:
    def test_sc_prc_12_successful_refund(self, processor):
        """SC-PRC-12: credit moves seller -> buyer."""
        processor._refund(Transaction("05", buyer="buyuser", seller="selluser",
                                      credit_cents=1000))
        assert processor._accounts.get_credit("selluser") == 0
        assert processor._accounts.get_credit("buyuser") == 6000

    def test_sc_prc_13_seller_cannot_cover_refund(self, processor, capsys):
        """SC-PRC-13: a seller without enough credit logs InsufficientCredit."""
        processor._refund(Transaction("05", buyer="buyuser", seller="selluser",
                                      credit_cents=1001))
        assert "InsufficientCredit" in capsys.readouterr().out
        assert processor._accounts.get_credit("selluser") == 1000


class TestAddCredit:
    def test_sc_prc_14_adds_credit(self, processor):
        """SC-PRC-14: the amount is added to the user's credit."""
        processor._add_credit(Transaction("06", username="buyuser", credit_cents=500))
        assert processor._accounts.get_credit("buyuser") == 5500

    def test_sc_prc_15_unknown_user_rejected(self, processor, capsys):
        """SC-PRC-15: adding credit to a user that does not exist logs an error instead of crashing."""
        processor._add_credit(Transaction("06", username="ghost", credit_cents=500))
        assert "UnknownUser" in capsys.readouterr().out

    def test_sc_prc_16_over_max_credit_rejected(self, processor, capsys):
        """SC-PRC-16: credit may not go above $999,999.00 (the system maximum)."""
        processor._add_credit(Transaction("06", username="buyuser", credit_cents=99_999_900))
        assert "MaxCredit" in capsys.readouterr().out
        assert processor._accounts.get_credit("buyuser") == 5000


class TestLoadFiles:
    def write_inputs(self, tmp_path):
        (tmp_path / "users.txt").write_text("bob             FS 000010.00\nEND\n")
        (tmp_path / "games.txt").write_text("Hades                      bob             025.00\nEND\n")
        (tmp_path / "collection.txt").write_text("Portal 2                   bob            \nEND\n")
        return [str(tmp_path / name) for name in ("users.txt", "games.txt", "collection.txt")]

    def test_sc_prc_17_loads_all_three_files(self, tmp_path):
        """SC-PRC-17: all three files are loaded when they are valid."""
        processor = TransactionProcessor(ErrorLogger())
        processor.load_files(*self.write_inputs(tmp_path))
        assert processor._accounts.get_credit("bob") == 1000
        assert processor._available_games.exists("Hades")
        assert processor._collection.owns("bob", "Portal 2")

    @pytest.mark.parametrize("bad_index", [0, 1, 2])
    def test_sc_prc_18_missing_file_is_fatal(self, tmp_path, capsys, bad_index):
        """SC-PRC-18: a missing accounts, games or collection file prints BadFile and exits."""
        paths = self.write_inputs(tmp_path)
        paths[bad_index] = str(tmp_path / "missing.txt")
        with pytest.raises(SystemExit):
            TransactionProcessor(ErrorLogger()).load_files(*paths)
        assert "BadFile" in capsys.readouterr().out


class TestSaveFiles:
    def test_sc_prc_19_writes_all_three_files(self, processor, tmp_path):
        """SC-PRC-19: all three output files are written."""
        paths = [str(tmp_path / name) for name in ("u.txt", "g.txt", "c.txt")]
        processor.save_files(*paths)
        assert "buyuser" in open(paths[0]).read()
        assert "Hades" in open(paths[1]).read()
        assert "Portal 2" in open(paths[2]).read()
