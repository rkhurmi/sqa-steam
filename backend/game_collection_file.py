from __future__ import annotations


class GameCollectionFile:
    def __init__(self) -> None:
        self._entries: set[tuple[str, str]] = set()

    def load(self, path: str) -> None:
        raise NotImplementedError

    def save(self, path: str) -> None:
        raise NotImplementedError

    def owns(self, username: str, game_name: str) -> bool:
        raise NotImplementedError

    def add_ownership(self, game_name: str, owner: str) -> None:
        raise NotImplementedError

    def remove_ownership_by_user(self, username: str) -> None:
        raise NotImplementedError
