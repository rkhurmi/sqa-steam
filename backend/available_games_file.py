from __future__ import annotations


class AvailableGamesFile:
    def __init__(self) -> None:
        self._games: dict[str, tuple[str, int]] = {}

    def load(self, path: str) -> None:
        raise NotImplementedError

    def save(self, path: str) -> None:
        raise NotImplementedError

    def exists(self, name: str) -> bool:
        raise NotImplementedError

    def add_game(self, name: str, seller: str, price_cents: int) -> None:
        raise NotImplementedError

    def remove_game(self, name: str) -> None:
        raise NotImplementedError

    def remove_games_by_seller(self, seller: str) -> None:
        raise NotImplementedError
