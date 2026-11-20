# =============================================================================
# Back End - Digital Games Distribution System (Phase 4 rapid prototype)
#
# WHAT IT DOES
#   Runs once at the end of each day. It reads the old accounts, available
#   games and game collection files, applies every transaction from the merged
#   daily transaction file in order, and writes new versions of the three files.
#   Transactions that break a rule (negative balance, duplicate username) are
#   reported as "ERROR: ..." on standard output and skipped. A missing or
#   unreadable file prints an error and stops the program with exit code 1.
#
# INPUT FILES
#   merged transaction file, old accounts file, old available games file,
#   old game collection file
# OUTPUT FILES
#   new accounts file, new available games file, new game collection file
#
# HOW TO RUN
#   python3 backend.py <transactions> <old_accounts> <old_games> \
#       <old_collection> <new_accounts> <new_games> <new_collection>
# =============================================================================

import sys

from error_logger import ErrorLogger
from transaction_processor import TransactionProcessor
from transaction_reader import TransactionReader

# Number of command line arguments the program needs (not counting the script name).
EXPECTED_ARGUMENT_COUNT = 7


class BackEnd:
    """Coordinates the whole run: load the old files, apply transactions, write the new files."""

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
        """
        Store the file paths and create the reader, processor and logger.

        Parameters:
            merged_transactions_path - merged daily transaction file (input)
            old_accounts_path        - old current user accounts file (input)
            old_games_path           - old available games file (input)
            old_collection_path      - old game collection file (input)
            new_accounts_path        - new current user accounts file (output)
            new_games_path           - new available games file (output)
            new_collection_path      - new game collection file (output)
        """
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
        """
        Run the whole back end: load, process, write.

        Returns nothing.
        """
        self.load_inputs()
        self.process_transactions()
        self.write_outputs()

    def load_inputs(self) -> None:
        """
        Load the three old data files into the processor.

        Returns nothing.
        """
        self._processor.load_files(
            self._old_accounts_path,
            self._old_games_path,
            self._old_collection_path,
        )

    def process_transactions(self) -> None:
        """
        Read the merged transaction file and apply each transaction in order.
        A missing or malformed transaction file is a fatal error.

        Returns nothing.
        """
        try:
            transactions = self._reader.read(self._merged_transactions_path)
        except Exception as error:
            self._logger.fatal_error("BadFile", str(error), self._merged_transactions_path)
        for transaction in transactions:
            self._processor.apply(transaction)

    def write_outputs(self) -> None:
        """
        Write the three new data files.

        Returns nothing.
        """
        self._processor.save_files(
            self._new_accounts_path,
            self._new_games_path,
            self._new_collection_path,
        )


def main() -> None:
    """
    Command line entry point: build a BackEnd from the arguments and run it.

    Returns nothing.
    """
    arguments = sys.argv[1:]
    if len(arguments) != EXPECTED_ARGUMENT_COUNT:
        print("usage: python3 backend.py <transactions> <old_accounts> "
              "<old_games> <old_collection> <new_accounts> <new_games> <new_collection>")
        sys.exit(1)
    BackEnd(*arguments).run()


if __name__ == "__main__":
    main()
