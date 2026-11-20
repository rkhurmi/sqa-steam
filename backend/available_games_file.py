# Text between two fields on one line, and the name on the closing line.
SEPARATOR = " "
END_MARKER = "END"

# Characters taken by the cents part of a money text (".00").
DECIMAL_PART_WIDTH = 3

# ---- Fixed-width field sizes of one game line: game_name seller price ----
GAME_NAME_WIDTH = 26
SELLER_WIDTH = 15
PRICE_WIDTH = 6   # e.g. 015.00

# Position of each field inside a line (computed from the widths above).
GAME_NAME_START = 0
SELLER_START = GAME_NAME_START + GAME_NAME_WIDTH + len(SEPARATOR)
PRICE_START = SELLER_START + SELLER_WIDTH + len(SEPARATOR)
GAME_LINE_WIDTH = PRICE_START + PRICE_WIDTH   # 49 characters


class AvailableGamesFile:
    """Holds every game that is for sale and reads / writes the available games file."""

    def __init__(self) -> None:
        """Create an empty list of games: game name -> (seller, price in cents)."""
        self._games: dict[str, tuple[str, int]] = {}

    def load(self, path: str) -> None:
        """
        Read the available games file into memory.

        Parameters:
            path - path of the available games file
        Returns nothing. Raises an exception if the file is missing or malformed.
        """
        self._games = {}
        with open(path) as games_file:
            for line in games_file:
                line = line.rstrip("\n")
                if line.strip() == "":
                    continue
                if line[:GAME_NAME_WIDTH].rstrip() == END_MARKER:
                    break
                # Cut each fixed-width field out of the line and drop its padding.
                name = line[GAME_NAME_START:GAME_NAME_START + GAME_NAME_WIDTH].rstrip()
                seller = line[SELLER_START:SELLER_START + SELLER_WIDTH].rstrip()
                price_text = line[PRICE_START:PRICE_START + PRICE_WIDTH]
                # Money text such as 015.00 -> 1500 cents.
                dollars_text, cents_text = price_text.split(".")
                price_cents = int(dollars_text) * 100 + int(cents_text)
                self._games[name] = (seller, price_cents)

    def save(self, path: str) -> None:
        """
        Write every game to a file in the fixed-width format, then the END line.

        Parameters:
            path - path of the file to create
        Returns nothing.
        """
        with open(path, "w") as games_file:
            for name, (seller, price_cents) in self._games.items():
                # CHECK: money is zero-padded to PRICE_WIDTH characters; an amount
                # too big for the field would come out too long.
                price_text = (f"{price_cents // 100:0{PRICE_WIDTH - DECIMAL_PART_WIDTH}d}"
                              f".{price_cents % 100:02d}")
                # CHECK: text fields are left-justified, padded with spaces on the right.
                line = (
                    name.ljust(GAME_NAME_WIDTH) + SEPARATOR
                    + seller.ljust(SELLER_WIDTH) + SEPARATOR
                    + price_text
                )
                games_file.write(line + "\n")
            # CHECK: END line padded to the full 49 characters like the fixture file.
            games_file.write(END_MARKER.ljust(GAME_LINE_WIDTH) + "\n")

    def exists(self, name: str) -> bool:
        """
        Tell whether a game with this name is for sale.

        Parameters:
            name - the game name to look for
        Returns True if the game is listed.
        """
        return name in self._games

    def add_game(self, name: str, seller: str, price_cents: int) -> None:
        """
        List a game for sale.

        Parameters:
            name        - game name
            seller      - username of the seller
            price_cents - price in cents
        Returns nothing.
        """
        self._games[name] = (seller, price_cents)

    def remove_game(self, name: str) -> None:
        """
        Take one game off the list.

        Parameters:
            name - game name
        Returns nothing.
        """
        del self._games[name]

    def remove_games_by_seller(self, seller: str) -> None:
        """
        Take every game sold by one user off the list.

        Parameters:
            seller - username of the seller
        Returns nothing.
        """
        self._games = {
            name: (game_seller, price_cents)
            for name, (game_seller, price_cents) in self._games.items()
            if game_seller != seller
        }
