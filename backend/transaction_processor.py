from accounts_file import AccountsFile
from available_games_file import AvailableGamesFile
from error_logger import ErrorLogger
from game_collection_file import GameCollectionFile
from transaction import Transaction


class TransactionProcessor:
    def __init__(self, logger: ErrorLogger) -> None:
        self._accounts: AccountsFile = AccountsFile()
        self._available_games: AvailableGamesFile = AvailableGamesFile()
        self._collection: GameCollectionFile = GameCollectionFile()
        self._logger: ErrorLogger = logger

    def apply(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _create(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _delete(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _sell(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _buy(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _refund(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def _add_credit(self, transaction: Transaction) -> None:
        raise NotImplementedError

    def load_files(self, accounts_path: str, games_path: str, collection_path: str) -> None:
        raise NotImplementedError

    def save_files(self, accounts_path: str, games_path: str, collection_path: str) -> None:
        raise NotImplementedError
