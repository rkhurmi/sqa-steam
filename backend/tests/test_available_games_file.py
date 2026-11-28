from available_games_file import AvailableGamesFile

# Sample lines in the fixed-width available games file format.
HADES_LINE = "Hades                      fulluser        025.00"
PORTAL_LINE = "Portal 2                   selluser        009.99"
END_LINE = "END                                        000.00"


class TestGamesInit:
    def test_sc_gam_01_starts_empty(self):
        """SC-GAM-01: a new AvailableGamesFile has no games."""
        assert AvailableGamesFile()._games == {}


class TestGamesLoad:
    def test_sc_gam_02_loads_games_until_end(self, tmp_path):
        """SC-GAM-02: blank lines are skipped, games are read, reading stops at END."""
        path = tmp_path / "games.txt"
        path.write_text(HADES_LINE + "\n\n" + PORTAL_LINE + "\n" + END_LINE + "\n")
        games = AvailableGamesFile()
        games.load(str(path))
        assert games._games == {"Hades": ("fulluser", 2500), "Portal 2": ("selluser", 999)}


class TestGamesSave:
    def test_sc_gam_03_writes_fixed_width_lines_and_end(self, tmp_path):
        """SC-GAM-03: each game is written in fixed-width format, followed by the END line."""
        games = AvailableGamesFile()
        games.add_game("Hades", "fulluser", 2500)
        path = tmp_path / "new_games.txt"
        games.save(str(path))
        assert path.read_text() == HADES_LINE + "\n" + "END".ljust(49) + "\n"


class TestGamesExists:
    def test_sc_gam_04_true_and_false(self):
        """SC-GAM-04: True for a listed game, False otherwise."""
        games = AvailableGamesFile()
        games.add_game("Hades", "fulluser", 2500)
        assert games.exists("Hades")
        assert not games.exists("Portal 2")


class TestGamesAddGame:
    def test_sc_gam_05_adds_game(self):
        """SC-GAM-05: the game is stored with its seller and price."""
        games = AvailableGamesFile()
        games.add_game("Hades", "fulluser", 2500)
        assert games._games["Hades"] == ("fulluser", 2500)


class TestGamesRemoveGame:
    def test_sc_gam_06_removes_game(self):
        """SC-GAM-06: the game is gone after removal."""
        games = AvailableGamesFile()
        games.add_game("Hades", "fulluser", 2500)
        games.remove_game("Hades")
        assert not games.exists("Hades")


class TestGamesRemoveGamesBySeller:
    def test_sc_gam_07_removes_only_that_sellers_games(self):
        """SC-GAM-07: games by the seller are removed; other sellers' games stay."""
        games = AvailableGamesFile()
        games.add_game("Hades", "fulluser", 2500)
        games.add_game("Portal 2", "selluser", 999)
        games.remove_games_by_seller("fulluser")
        assert games._games == {"Portal 2": ("selluser", 999)}
