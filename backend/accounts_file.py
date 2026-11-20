from __future__ import annotations


class AccountsFile:
    def __init__(self) -> None:
        self._users: dict[str, tuple[str, int]] = {}

    def load(self, path: str) -> None:
        raise NotImplementedError

    def save(self, path: str) -> None:
        raise NotImplementedError

    def exists(self, username: str) -> bool:
        raise NotImplementedError

    def add_user(self, username: str, type: str, credit_cents: int) -> None:
        raise NotImplementedError

    def remove_user(self, username: str) -> None:
        raise NotImplementedError

    def get_credit(self, username: str) -> int:
        raise NotImplementedError

    def change_credit(self, username: str, amount_cents: int) -> None:
        raise NotImplementedError
