from game_collection_file import GameCollectionFile

# Sample lines in the fixed-width game collection file format.
HADES_LINE = "Hades                      buyuser        "
PORTAL_LINE = "Portal 2                   fulluser       "
END_LINE = "END".ljust(42)


class TestCollectionInit:
    def test_sc_col_01_starts_empty(self):
        """SC-COL-01: a new GameCollectionFile has no entries."""
        assert GameCollectionFile()._entries == set()


class TestCollectionLoad:
    def test_sc_col_02_loads_entries_until_end(self, tmp_path):
        """SC-COL-02: blank lines are skipped, entries are read, reading stops at END."""
        path = tmp_path / "collection.txt"
        path.write_text(HADES_LINE + "\n\n" + PORTAL_LINE + "\n" + END_LINE + "\n")
        collection = GameCollectionFile()
        collection.load(str(path))
        assert collection._entries == {("Hades", "buyuser"), ("Portal 2", "fulluser")}


class TestCollectionSave:
    def test_sc_col_03_writes_sorted_lines_and_end(self, tmp_path):
        """SC-COL-03: entries are written in sorted order, followed by the END line."""
        collection = GameCollectionFile()
        collection.add_ownership("Portal 2", "fulluser")
        collection.add_ownership("Hades", "buyuser")
        path = tmp_path / "new_collection.txt"
        collection.save(str(path))
        assert path.read_text() == HADES_LINE + "\n" + PORTAL_LINE + "\n" + END_LINE + "\n"


class TestCollectionOwns:
    def test_sc_col_04_true_and_false(self):
        """SC-COL-04: True only for an owned (user, game) pair."""
        collection = GameCollectionFile()
        collection.add_ownership("Hades", "buyuser")
        assert collection.owns("buyuser", "Hades")
        assert not collection.owns("fulluser", "Hades")


class TestCollectionAddOwnership:
    def test_sc_col_05_adds_entry(self):
        """SC-COL-05: the (game, owner) pair is stored."""
        collection = GameCollectionFile()
        collection.add_ownership("Hades", "buyuser")
        assert ("Hades", "buyuser") in collection._entries


class TestCollectionRemoveOwnershipByUser:
    def test_sc_col_06_removes_only_that_users_entries(self):
        """SC-COL-06: the user's games are removed; other users' games stay."""
        collection = GameCollectionFile()
        collection.add_ownership("Hades", "buyuser")
        collection.add_ownership("Portal 2", "buyuser")
        collection.add_ownership("Hades", "fulluser")
        collection.remove_ownership_by_user("buyuser")
        assert collection._entries == {("Hades", "fulluser")}
