from __future__ import annotations


from error_logger import ErrorLogger
from transaction_processor import TransactionProcessor
from transaction_reader import TransactionReader


class BackEnd:
    def __init__(
        self,
        merged_transactions_path: str,
        old_accounts_path: str,
        old_games_path: str,
        old_collection_path: str,
        new_accounts_path: str,
        new_games_path: str,
        new_collection_path: str,
    ) -> None:
        self._merged_transactions_path: str = merged_transactions_path
        self._old_accounts_path: str = old_accounts_path
        self._old_games_path: str = old_games_path
        self._old_collection_path: str = old_collection_path
        self._new_accounts_path: str = new_accounts_path
        self._new_games_path: str = new_games_path
        self._new_collection_path: str = new_collection_path
        self._logger: ErrorLogger = ErrorLogger()
        self._reader: TransactionReader = TransactionReader(self._logger)
        self._processor: TransactionProcessor = TransactionProcessor(self._logger)

    def run(self) -> None:
        raise NotImplementedError

    def load_inputs(self) -> None:
        raise NotImplementedError

    def process_transactions(self) -> None:
        raise NotImplementedError

    def write_outputs(self) -> None:
        raise NotImplementedError
