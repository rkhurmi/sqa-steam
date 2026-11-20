from transaction import Transaction


class ErrorLogger:
    def __init__(self) -> None:
        self.field: type = str

    def constraint_error(self, type: str, description: str, transaction: Transaction) -> None:
        raise NotImplementedError

    def fatal_error(self, type: str, description: str, file_name: str) -> None:
        raise NotImplementedError
