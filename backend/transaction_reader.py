from error_logger import ErrorLogger
from transaction import Transaction


class TransactionReader:
    def __init__(self, logger: ErrorLogger) -> None:
        self._logger: ErrorLogger = logger

    def read(self, path: str) -> list[Transaction]:
        raise NotImplementedError

    def parse_line(self, line: str) -> Transaction:
        raise NotImplementedError
