from accounts_file import AccountsFile
from available_games_file import AvailableGamesFile
from error_logger import ErrorLogger
from game_collection_file import GameCollectionFile
from transaction import Transaction

# ---- Transaction codes ----
CODE_CREATE = "01"
CODE_DELETE = "02"
CODE_SELL = "03"
CODE_BUY = "04"
CODE_REFUND = "05"
CODE_ADD_CREDIT = "06"

# Highest credit a user may hold: $999,999.00, in cents.
MAX_CREDIT_CENTS = 99_999_900


class TransactionProcessor:
    """Applies transactions, one at a time, to the accounts, games and collection."""

    def __init__(self, logger: ErrorLogger) -> None:
        """
        Create the processor with empty data files.

        Parameters:
            logger - the shared ErrorLogger
        """
        self._accounts: AccountsFile = AccountsFile()
        self._available_games: AvailableGamesFile = AvailableGamesFile()
        self._collection: GameCollectionFile = GameCollectionFile()
        self._logger: ErrorLogger = logger

    def apply(self, transaction: Transaction) -> None:
        """
        Apply one transaction by calling the handler that matches its code.

        Parameters:
            transaction - the transaction to apply
        Returns nothing.
        """
        if transaction.code == CODE_CREATE:
            self._create(transaction)
        elif transaction.code == CODE_DELETE:
            self._delete(transaction)
        elif transaction.code == CODE_SELL:
            self._sell(transaction)
        elif transaction.code == CODE_BUY:
            self._buy(transaction)
        elif transaction.code == CODE_REFUND:
            self._refund(transaction)
        elif transaction.code == CODE_ADD_CREDIT:
            self._add_credit(transaction)

    def _create(self, transaction: Transaction) -> None:
        """
        01 create: add a new account unless the username is already taken.

        Parameters:
            transaction - holds username, user_type and credit_cents
        Returns nothing.
        """
        if self._accounts.exists(transaction.username):
            self._logger.constraint_error(
                "DuplicateUsername",
                f"user {transaction.username} already exists",
                transaction,
            )
            return
        self._accounts.add_user(transaction.username, transaction.user_type,
                                transaction.credit_cents)

    def _delete(self, transaction: Transaction) -> None:
        """
        02 delete: remove an account and everything that belongs to it.

        Parameters:
            transaction - holds the username to delete
        Returns nothing.
        """
        if not self._accounts.exists(transaction.username):
            self._logger.constraint_error(
                "UnknownUser",
                f"user {transaction.username} does not exist",
                transaction,
            )
            return
        self._available_games.remove_games_by_seller(transaction.username)
        # ASSUMPTION: the deleted user's collection entries are removed too
        # (pending TA confirmation).
        self._collection.remove_ownership_by_user(transaction.username)
        self._accounts.remove_user(transaction.username)

    def _sell(self, transaction: Transaction) -> None:
        """
        03 sell: put a game on the available games list.

        Parameters:
            transaction - holds game_name, seller and price_cents
        Returns nothing.
        """
        self._available_games.add_game(transaction.game_name, transaction.seller,
                                       transaction.price_cents)

    def _buy(self, transaction: Transaction) -> None:
        """
        04 buy: the buyer pays the seller and receives the game. The game
        stays on the available games list.

        Parameters:
            transaction - holds game_name, seller, buyer and price_cents
        Returns nothing.
        """
        if self._collection.owns(transaction.buyer, transaction.game_name):
            self._logger.constraint_error(
                "AlreadyOwned",
                f"buyer {transaction.buyer} already owns {transaction.game_name}",
                transaction,
            )
            return
        if self._accounts.get_credit(transaction.buyer) < transaction.price_cents:
            self._logger.constraint_error(
                "InsufficientCredit",
                f"buyer {transaction.buyer} cannot afford {transaction.game_name}",
                transaction,
            )
            return
        self._accounts.change_credit(transaction.buyer, -transaction.price_cents)
        self._accounts.change_credit(transaction.seller, transaction.price_cents)
        self._collection.add_ownership(transaction.game_name, transaction.buyer)

    def _refund(self, transaction: Transaction) -> None:
        """
        05 refund: move credit from the seller back to the buyer.

        Parameters:
            transaction - holds buyer, seller and credit_cents
        Returns nothing.
        """
        if self._accounts.get_credit(transaction.seller) < transaction.credit_cents:
            self._logger.constraint_error(
                "InsufficientCredit",
                f"seller {transaction.seller} cannot cover the refund",
                transaction,
            )
            return
        self._accounts.change_credit(transaction.seller, -transaction.credit_cents)
        self._accounts.change_credit(transaction.buyer, transaction.credit_cents)

    def _add_credit(self, transaction: Transaction) -> None:
        """
        06 addcredit: add money to a user's account.

        Parameters:
            transaction - holds username and credit_cents
        Returns nothing.
        """
        if not self._accounts.exists(transaction.username):
            self._logger.constraint_error(
                "UnknownUser",
                f"user {transaction.username} does not exist",
                transaction,
            )
            return
        new_credit = self._accounts.get_credit(transaction.username) + transaction.credit_cents
        if new_credit > MAX_CREDIT_CENTS:
            self._logger.constraint_error(
                "MaxCredit",
                f"user {transaction.username} would go over the maximum credit",
                transaction,
            )
            return
        self._accounts.change_credit(transaction.username, transaction.credit_cents)

    def load_files(self, accounts_path: str, games_path: str, collection_path: str) -> None:
        """
        Load the three old data files. A bad or missing file is a fatal error.

        Parameters:
            accounts_path   - current user accounts file
            games_path      - available games file
            collection_path - game collection file
        Returns nothing.
        """
        try:
            self._accounts.load(accounts_path)
        except Exception as error:
            self._logger.fatal_error("BadFile", str(error), accounts_path)
        try:
            self._available_games.load(games_path)
        except Exception as error:
            self._logger.fatal_error("BadFile", str(error), games_path)
        try:
            self._collection.load(collection_path)
        except Exception as error:
            self._logger.fatal_error("BadFile", str(error), collection_path)

    def save_files(self, accounts_path: str, games_path: str, collection_path: str) -> None:
        """
        Write the three new data files.

        Parameters:
            accounts_path   - new current user accounts file
            games_path      - new available games file
            collection_path - new game collection file
        Returns nothing.
        """
        self._accounts.save(accounts_path)
        self._available_games.save(games_path)
        self._collection.save(collection_path)
