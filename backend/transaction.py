from dataclasses import dataclass


@dataclass
class Transaction:
    """
    One line of the daily transaction file, as plain data.

    `code` is always set. The other fields are only filled in when the
    transaction code uses them; the rest stay None. All money is in cents.
    """

    code: str
    username: str | None = None
    user_type: str | None = None
    credit_cents: int | None = None
    game_name: str | None = None
    seller: str | None = None
    buyer: str | None = None
    price_cents: int | None = None
