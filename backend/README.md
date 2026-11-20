# Digital Games Distribution System

## Back End (`backend/`)

The back end is a Python batch program that runs once at the end of each day. It reads the previous day's data files and the merged daily transaction file, applies every transaction in order, and writes updated data files.

### Running

```
python3 backend/backend.py <merged_transactions> <old_accounts> <old_games> \
    <old_collection> <new_accounts> <new_games> <new_collection>
```

Takes exactly 7 arguments (3 inputs + the transaction file, then 3 output paths). Otherwise it prints a usage message and exits with code 1.

### Files

| File | Role |
|---|---|
| `backend.py` | Entry point. `BackEnd` loads the old files, processes transactions, and writes the new files. |
| `transaction_reader.py` | `TransactionReader` parses the fixed-width transaction file into `Transaction` objects. Money is converted to cents. |
| `transaction.py` | `Transaction` dataclass: a code plus the optional fields that code uses. |
| `transaction_processor.py` | `TransactionProcessor` holds the three data files in memory and applies each transaction by its code. |
| `accounts_file.py` | `AccountsFile`: load/save the user accounts (username, type, credit). |
| `available_games_file.py` | `AvailableGamesFile`: load/save games for sale (name, seller, price). |
| `game_collection_file.py` | `GameCollectionFile`: load/save which user owns which game. |
| `error_logger.py` | `ErrorLogger`: prints `ERROR: ...` messages to standard output. |

### Transaction codes

| Code | Action | Rule checked |
|---|---|---|
| 01 | Create account | Username must not already exist |
| 02 | Delete account | Also removes the user's games for sale and owned games |
| 03 | Sell | Adds the game to the available games list |
| 04 | Buy | Buyer must not already own the game and must have enough credit; credit moves from buyer to seller |
| 05 | Refund | Seller must have enough credit; credit moves from seller to buyer |
| 06 | Add credit | Adds to the user's balance |
| 00 | End of session | Ignored by the processor |

### Error handling

- **Constraint errors** (duplicate username, insufficient credit, game already owned) are printed as `ERROR: <type>: <description> (transaction code NN)`. The transaction is skipped and processing continues.
- **Fatal errors** (a missing or malformed input file) are printed as `ERROR: BadFile: ... (file <path>)` and the program exits with code 1. No output files are written.
