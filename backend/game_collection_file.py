# Text between two fields on one line, and the name on the closing line.
SEPARATOR = " "
END_MARKER = "END"

# ---- Fixed-width field sizes of one collection line: game_name owner ----
GAME_NAME_WIDTH = 26
OWNER_WIDTH = 15

# Position of each field inside a line (computed from the widths above).
GAME_NAME_START = 0
OWNER_START = GAME_NAME_START + GAME_NAME_WIDTH + len(SEPARATOR)
COLLECTION_LINE_WIDTH = OWNER_START + OWNER_WIDTH   # 42 characters


class GameCollectionFile:
    """Holds which user owns which game and reads / writes the game collection file."""

    def __init__(self) -> None:
        """Create an empty collection: a set of (game name, owner) pairs."""
        self._entries: set[tuple[str, str]] = set()

    def load(self, path: str) -> None:
        """
        Read the game collection file into memory.

        Parameters:
            path - path of the game collection file
        Returns nothing. Raises an exception if the file is missing or malformed.
        """
        self._entries = set()
        with open(path) as collection_file:
            for line in collection_file:
                line = line.rstrip("\n")
                if line.strip() == "":
                    continue
                if line[:GAME_NAME_WIDTH].rstrip() == END_MARKER:
                    break
                # Cut each fixed-width field out of the line and drop its padding.
                game_name = line[GAME_NAME_START:GAME_NAME_START + GAME_NAME_WIDTH].rstrip()
                owner = line[OWNER_START:OWNER_START + OWNER_WIDTH].rstrip()
                self._entries.add((game_name, owner))

    def save(self, path: str) -> None:
        """
        Write every ownership record to a file in the fixed-width format, then the END line.

        Parameters:
            path - path of the file to create
        Returns nothing.
        """
        with open(path, "w") as collection_file:
            # Sorted so the output file order is always the same.
            for game_name, owner in sorted(self._entries):
                # CHECK: text fields are left-justified, padded with spaces on the right.
                line = game_name.ljust(GAME_NAME_WIDTH) + SEPARATOR + owner.ljust(OWNER_WIDTH)
                collection_file.write(line + "\n")
            # CHECK: END line padded to the full 42 characters like the fixture file.
            collection_file.write(END_MARKER.ljust(COLLECTION_LINE_WIDTH) + "\n")

    def owns(self, username: str, game_name: str) -> bool:
        """
        Tell whether a user owns a game.

        Parameters:
            username  - the user
            game_name - the game
        Returns True if the pair is in the collection.
        """
        return (game_name, username) in self._entries

    def add_ownership(self, game_name: str, owner: str) -> None:
        """
        Record that a user now owns a game.

        Parameters:
            game_name - the game
            owner     - username of the new owner
        Returns nothing.
        """
        self._entries.add((game_name, owner))

    def remove_ownership_by_user(self, username: str) -> None:
        """
        Remove every game owned by one user.

        Parameters:
            username - the user
        Returns nothing.
        """
        self._entries = {entry for entry in self._entries if entry[1] != username}
