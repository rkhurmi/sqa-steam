import os
import sys

import pytest

import backend
from backend import BackEnd

DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data")


def sample_paths(tmp_path):
    """The 7 BackEnd arguments: the sample inputs in backend/data and outputs in tmp_path."""
    return [
        os.path.join(DATA_DIR, "daily_transactions.txt"),
        os.path.join(DATA_DIR, "users.txt"),
        os.path.join(DATA_DIR, "games.txt"),
        os.path.join(DATA_DIR, "collection.txt"),
        str(tmp_path / "new_users.txt"),
        str(tmp_path / "new_games.txt"),
        str(tmp_path / "new_collection.txt"),
    ]


class TestBackEndInit:
    def test_sc_be_01_stores_paths(self, tmp_path):
        """SC-BE-01: the paths are stored and the helpers are created."""
        paths = sample_paths(tmp_path)
        back_end = BackEnd(*paths)
        assert back_end._merged_transactions_path == paths[0]
        assert back_end._new_collection_path == paths[6]
        assert back_end._reader is not None and back_end._processor is not None


class TestRun:
    def test_sc_be_02_full_run_writes_new_files(self, tmp_path):
        """SC-BE-02: a full run on the sample data writes the expected new files."""
        BackEnd(*sample_paths(tmp_path)).run()
        users = (tmp_path / "new_users.txt").read_text()
        assert "newuser" in users and "pooruser" not in users
        assert "New Game" in (tmp_path / "new_games.txt").read_text()
        assert "Stardew Valley             buyuser" in (tmp_path / "new_collection.txt").read_text()


class TestLoadInputs:
    def test_sc_be_03_loads_old_files(self, tmp_path):
        """SC-BE-03: the old accounts file is loaded into the processor."""
        back_end = BackEnd(*sample_paths(tmp_path))
        back_end.load_inputs()
        assert back_end._processor._accounts.exists("admin")


class TestProcessTransactions:
    def test_sc_be_04_applies_transactions(self, tmp_path):
        """SC-BE-04: transactions from the file are applied (newuser is created)."""
        back_end = BackEnd(*sample_paths(tmp_path))
        back_end.load_inputs()
        back_end.process_transactions()
        assert back_end._processor._accounts.exists("newuser")

    def test_sc_be_05_missing_transaction_file_is_fatal(self, tmp_path, capsys):
        """SC-BE-05: a missing transaction file prints BadFile and exits."""
        paths = sample_paths(tmp_path)
        paths[0] = str(tmp_path / "missing.txt")
        with pytest.raises(SystemExit):
            BackEnd(*paths).process_transactions()
        assert "BadFile" in capsys.readouterr().out


class TestWriteOutputs:
    def test_sc_be_06_writes_files(self, tmp_path):
        """SC-BE-06: the three output files are created."""
        BackEnd(*sample_paths(tmp_path)).write_outputs()
        for name in ("new_users.txt", "new_games.txt", "new_collection.txt"):
            assert (tmp_path / name).exists()


class TestMain:
    def test_sc_be_07_wrong_argument_count_exits(self, monkeypatch, capsys):
        """SC-BE-07: the wrong number of arguments prints usage and exits with code 1."""
        monkeypatch.setattr(sys, "argv", ["backend.py", "only_one_arg"])
        with pytest.raises(SystemExit) as exit_info:
            backend.main()
        assert exit_info.value.code == 1
        assert "usage" in capsys.readouterr().out

    def test_sc_be_08_correct_arguments_run(self, monkeypatch, tmp_path):
        """SC-BE-08: 7 arguments run the back end and write the output files."""
        monkeypatch.setattr(sys, "argv", ["backend.py"] + sample_paths(tmp_path))
        backend.main()
        assert (tmp_path / "new_users.txt").exists()
