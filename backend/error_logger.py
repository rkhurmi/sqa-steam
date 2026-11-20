import sys

from transaction import Transaction


FATAL_EXIT_CODE = 1


class ErrorLogger:
    """Prints error messages to standard output in the form ERROR: <message>."""

    def constraint_error(self, type: str, description: str, transaction: Transaction) -> None:
        """
        Report a transaction that broke a business rule. Processing continues.

        Parameters:
            type        - short name of the error, e.g. "InsufficientCredit"
            description - human readable explanation
            transaction - the transaction that was rejected
        Returns nothing.
        """
        print(f"ERROR: {type}: {description} (transaction code {transaction.code})")

    def fatal_error(self, type: str, description: str, file_name: str) -> None:
        """
        Report an unrecoverable file problem and stop the program at once.

        Parameters:
            type        - short name of the error, e.g. "BadFile"
            description - human readable explanation
            file_name   - the file that caused the problem
        Returns nothing; the program exits with a non-zero exit code.
        """
        print(f"ERROR: {type}: {description} (file {file_name})")
        sys.exit(FATAL_EXIT_CODE)
