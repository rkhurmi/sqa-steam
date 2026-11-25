from transaction import Transaction


class TestTransaction:
    def test_sc_trn_01_unused_fields_default_to_none(self):
        """SC-TRN-01: only the code is required; every other field starts as None."""
        transaction = Transaction("06", username="bob", credit_cents=500)
        assert transaction.code == "06"
        assert transaction.username == "bob"
        assert transaction.credit_cents == 500
        assert transaction.game_name is None
        assert transaction.buyer is None
