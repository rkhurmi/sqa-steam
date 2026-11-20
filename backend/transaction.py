from __future__ import annotations


from dataclasses import dataclass


@dataclass
class Transaction:
    code: str
    username: str | None = None
    user_type: str | None = None
    credit_cents: int | None = None
    game_name: str | None = None
    seller: str | None = None
    buyer: str | None = None
    price_cents: int | None = None
