# CSCI 3060U Phase 5: Back End Unit Testing - Test Report

## 1. Overview
White-box unit tests for the Python Back End (`backend/`). They are written with pytest and run in PyCharm.

- **Statement coverage:** every method in every Back End module.
- **Path coverage:** `AccountsFile.load` (`backend/accounts_file.py`). See Section 4.
- **Tests:** 70 test functions (77 test runs, because two tests are parametrized), in `backend/tests/`, one file per module and one test class per method.

**How to run**
- PyCharm: right-click `backend/tests` and choose **Run 'pytest in tests'**.
- Coverage (PyCharm terminal): `pytest backend/tests --cov=backend --cov-report=term-missing`
- Terminal (from the project root): `pytest backend/tests -v --cov=backend --cov-report=term-missing`

`backend/tests/conftest.py` adds `backend/` to the import path, so the tests use the Back End's own imports without any extra setup.

## 2. Test Cases and Results
One table per module, listing every method with its test cases. **SC** = statement coverage. **PC** = path coverage for **AccountsFile.load** (labelled **PATH**).

### error_logger.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-LOG-01 | ErrorLogger.constraint_error | Statement | type "DuplicateUsername", description "user bob already exists", transaction code 01 | Prints `ERROR: DuplicateUsername: user bob already exists (transaction code 01)`; program continues | Pass | Pass |
| SC-LOG-02 | ErrorLogger.fatal_error | Statement | type "BadFile", description "missing", file "users.txt" | Prints `ERROR: BadFile: missing (file users.txt)`; exits with code 1 | Pass | Pass |

### transaction.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-TRN-01 | Transaction (dataclass) | Statement | code "06", username "bob", credit 500 cents | Given fields are stored; unused fields (game_name, buyer) are None | Pass | Pass |

### transaction_reader.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-RDR-01 | read_field | Statement | line `01 bob             FS`, start 3, width 15 | `bob` (padding removed) | Pass | Pass |
| SC-RDR-02 | money_to_cents | Statement | `001000.50` | 100050 | Pass | Pass |
| SC-RDR-03 | TransactionReader.__init__ | Statement | an ErrorLogger | Reader keeps that logger | Pass | Pass |
| SC-RDR-04 | TransactionReader.read | Statement | File: create line, blank line, 00 line, sell line | Two transactions, codes 01 and 03 (blank and 00 skipped) | Pass | Pass |
| SC-RDR-05 | TransactionReader.parse_line | Statement | `01 newuser         FS 000050.00` | code 01, username newuser, type FS, credit 5000 | Pass | Pass |
| SC-RDR-06 | TransactionReader.parse_line | Statement | `05 buyuser         selluser        000010.00` | buyer buyuser, seller selluser, credit 1000 | Pass | Pass |
| SC-RDR-07 | TransactionReader.parse_line | Statement | `03 New Game ... selluser 019.99` | game New Game, seller selluser, price 1999 | Pass | Pass |
| SC-RDR-08 | TransactionReader.parse_line | Statement | `04 Stardew Valley ... selluser buyuser 015.00` | game Stardew Valley, seller selluser, buyer buyuser, price 1500 | Pass | Pass |
| SC-RDR-09 | TransactionReader.parse_line | Statement | `99 something` | ValueError raised | Pass | Pass |

### accounts_file.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-ACC-01 | AccountsFile.__init__ | Statement | (none) | No users | Pass | Pass |
| SC-ACC-02 | AccountsFile.load | Statement | File: admin line, blank line, bob line, END | admin (AA, 100000) and bob (FS, 2550) loaded | Pass | Pass |
| SC-ACC-03 | AccountsFile.save | Statement | One user: admin, AA, 100000 | File is `admin           AA 001000.00` then END padded to 28 chars | Pass | Pass |
| SC-ACC-04 | AccountsFile.exists | Statement | User bob exists; ask for bob and alice | True for bob, False for alice | Pass | Pass |
| SC-ACC-05 | AccountsFile.add_user | Statement | bob, BS, 500 | bob stored as (BS, 500) | Pass | Pass |
| SC-ACC-06 | AccountsFile.remove_user | Statement | Existing user bob | bob no longer exists | Pass | Pass |
| SC-ACC-07 | AccountsFile.get_credit | Statement | bob with 1234 cents | 1234 | Pass | Pass |
| SC-ACC-08 | AccountsFile.change_credit | Statement | bob (FS, 1000), change by -250 | bob is (FS, 750) | Pass | Pass |
| PC-01 | AccountsFile.load | **PATH** | Empty file | No users | Pass | Pass |
| PC-02 | AccountsFile.load | **PATH** | END only | No users | Pass | Pass |
| PC-03 | AccountsFile.load | **PATH** | One blank line, no END | No users | Pass | Pass |
| PC-04 | AccountsFile.load | **PATH** | bob line, no END | bob (FS, 2550) | Pass | Pass |
| PC-05 | AccountsFile.load | **PATH** | Blank line, END | No users | Pass | Pass |
| PC-06 | AccountsFile.load | **PATH** | bob line, END | bob (FS, 2550) | Pass | Pass |
| PC-07 | AccountsFile.load | **PATH** | admin line, bob line, END, extra user line | admin and bob only (line after END ignored) | Pass | Pass |
| PC-08 | AccountsFile.load | **PATH** | Blank line, bob line, END | bob (FS, 2550) | Pass | Pass |
| PC-09 | AccountsFile.load | **PATH** | Path to a missing file | FileNotFoundError raised | Pass | Pass |
| PC-10 | AccountsFile.load | **PATH** | bob line with credit `000002550` (no dot), END | ValueError raised | Pass | Pass |

### available_games_file.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-GAM-01 | AvailableGamesFile.__init__ | Statement | (none) | No games | Pass | Pass |
| SC-GAM-02 | AvailableGamesFile.load | Statement | File: Hades line, blank line, Portal 2 line, END | Hades (fulluser, 2500) and Portal 2 (selluser, 999) loaded | Pass | Pass |
| SC-GAM-03 | AvailableGamesFile.save | Statement | One game: Hades, fulluser, 2500 | File is the Hades line then END padded to 49 chars | Pass | Pass |
| SC-GAM-04 | AvailableGamesFile.exists | Statement | Hades listed; ask for Hades and Portal 2 | True for Hades, False for Portal 2 | Pass | Pass |
| SC-GAM-05 | AvailableGamesFile.add_game | Statement | Hades, fulluser, 2500 | Hades stored as (fulluser, 2500) | Pass | Pass |
| SC-GAM-06 | AvailableGamesFile.remove_game | Statement | Listed game Hades | Hades no longer listed | Pass | Pass |
| SC-GAM-07 | AvailableGamesFile.remove_games_by_seller | Statement | Hades (fulluser), Portal 2 (selluser); remove fulluser | Only Portal 2 remains | Pass | Pass |

### game_collection_file.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-COL-01 | GameCollectionFile.__init__ | Statement | (none) | No entries | Pass | Pass |
| SC-COL-02 | GameCollectionFile.load | Statement | File: Hades/buyuser, blank line, Portal 2/fulluser, END | Both entries loaded | Pass | Pass |
| SC-COL-03 | GameCollectionFile.save | Statement | Portal 2/fulluser added before Hades/buyuser | File lists Hades first, then Portal 2 (sorted), then END padded to 42 chars | Pass | Pass |
| SC-COL-04 | GameCollectionFile.owns | Statement | buyuser owns Hades; ask buyuser and fulluser | True for buyuser, False for fulluser | Pass | Pass |
| SC-COL-05 | GameCollectionFile.add_ownership | Statement | Hades, buyuser | (Hades, buyuser) stored | Pass | Pass |
| SC-COL-06 | GameCollectionFile.remove_ownership_by_user | Statement | buyuser owns Hades and Portal 2, fulluser owns Hades; remove buyuser | Only (Hades, fulluser) remains | Pass | Pass |

### transaction_processor.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-PRC-01 | TransactionProcessor.__init__ | Statement | an ErrorLogger | Empty accounts, games and collection; keeps the logger | Pass | Pass |
| SC-PRC-02 | TransactionProcessor.apply | Statement | Codes 01, 02, 03, 04, 05, 06 (one run each) | The matching handler (_create ... _add_credit) is called | Pass | Pass |
| SC-PRC-03 | TransactionProcessor.apply | Statement | Code 00 | Nothing changes | Pass | Pass |
| SC-PRC-04 | TransactionProcessor._create | Statement | 01 newuser FS 0 | newuser exists | Pass | Pass |
| SC-PRC-05 | TransactionProcessor._create | Statement | 01 buyuser (already exists) | `DuplicateUsername` error; buyuser unchanged | Pass | Pass |
| SC-PRC-06 | TransactionProcessor._delete | Statement | 02 selluser, then 02 buyuser | Users removed; selluser's game Hades unlisted; buyuser's Portal 2 removed from collection | Pass | Pass |
| SC-PRC-07 | TransactionProcessor._delete | Statement | 02 ghost (does not exist) | `UnknownUser` error; no crash | **FAIL** | Pass |
| SC-PRC-08 | TransactionProcessor._sell | Statement | 03 Celeste selluser 19.99 | Celeste listed as (selluser, 1999) | Pass | Pass |
| SC-PRC-09 | TransactionProcessor._buy | Statement | 04 Hades selluser buyuser 25.00 (buyer has 50.00) | buyuser 25.00, selluser 35.00, buyuser owns Hades | Pass | Pass |
| SC-PRC-10 | TransactionProcessor._buy | Statement | 04 Portal 2 for buyuser (already owns it) | `AlreadyOwned` error; credit unchanged | Pass | Pass |
| SC-PRC-11 | TransactionProcessor._buy | Statement | 04 Hades for selluser at 25.00 (has 10.00) | `InsufficientCredit` error; game not added | Pass | Pass |
| SC-PRC-12 | TransactionProcessor._refund | Statement | 05 buyuser selluser 10.00 (seller has 10.00) | selluser 0.00, buyuser 60.00 | Pass | Pass |
| SC-PRC-13 | TransactionProcessor._refund | Statement | 05 buyuser selluser 10.01 (seller has 10.00) | `InsufficientCredit` error; seller unchanged | Pass | Pass |
| SC-PRC-14 | TransactionProcessor._add_credit | Statement | 06 buyuser 5.00 | buyuser 55.00 | Pass | Pass |
| SC-PRC-15 | TransactionProcessor._add_credit | Statement | 06 ghost 5.00 (does not exist) | `UnknownUser` error; no crash | **FAIL** | Pass |
| SC-PRC-16 | TransactionProcessor._add_credit | Statement | 06 buyuser 999,999.00 (has 50.00) | `MaxCredit` error; credit stays 50.00 | **FAIL** | Pass |
| SC-PRC-17 | TransactionProcessor.load_files | Statement | Valid accounts, games and collection files | bob (10.00), Hades listed, bob owns Portal 2 | Pass | Pass |
| SC-PRC-18 | TransactionProcessor.load_files | Statement | Accounts, games or collection path missing (one run each) | `BadFile` error; program exits | Pass | Pass |
| SC-PRC-19 | TransactionProcessor.save_files | Statement | Processor with buyuser, Hades, Portal 2 | All three output files written with those entries | Pass | Pass |

### backend.py

| Test ID | Method | Coverage | Input | Expected output | First run | After fixes |
|---|---|---|---|---|---|---|
| SC-BE-01 | BackEnd.__init__ | Statement | 7 file paths | Paths stored; reader and processor created | Pass | Pass |
| SC-BE-02 | BackEnd.run | Statement | Sample files in backend/data | newuser added, pooruser removed, New Game listed, buyuser owns Stardew Valley | Pass | Pass |
| SC-BE-03 | BackEnd.load_inputs | Statement | Sample files in backend/data | admin is loaded | Pass | Pass |
| SC-BE-04 | BackEnd.process_transactions | Statement | Sample transaction file | newuser is created | Pass | Pass |
| SC-BE-05 | BackEnd.process_transactions | Statement | Missing transaction file | `BadFile` error; program exits | Pass | Pass |
| SC-BE-06 | BackEnd.write_outputs | Statement | Empty data | Three output files created | Pass | Pass |
| SC-BE-07 | main | Statement | 1 command line argument | Usage message; exit code 1 | Pass | Pass |
| SC-BE-08 | main | Statement | 7 command line arguments (sample files) | Back End runs; new_users.txt written | Pass | Pass |

**Summary:** 70 test cases (77 test runs, because SC-PRC-02 and SC-PRC-18 are parametrized). First run: 74 passed, 3 failed. After fixes: 77 passed, 0 failed.

## 3. Statement Coverage Results
Results from `pytest --cov=backend --cov-report=term-missing` after the fixes:

| Module | Statements | Missed | Coverage |
|---|---|---|---|
| accounts_file.py | 46 | 0 | 100% |
| available_games_file.py | 43 | 0 | 100% |
| backend.py | 40 | 1 | 98% |
| error_logger.py | 9 | 0 | 100% |
| game_collection_file.py | 34 | 0 | 100% |
| transaction.py | 11 | 0 | 100% |
| transaction_processor.py | 87 | 0 | 100% |
| transaction_reader.py | 71 | 0 | 100% |

The one missed statement in `backend.py` (line 134) is the `main()` call inside `if __name__ == "__main__":`. It is not part of any method and only runs when the file is started as a script. `main()` itself is fully covered by SC-BE-07 and SC-BE-08.

Coverage was measured with pytest-cov from the PyCharm terminal: `pytest backend/tests --cov=backend --cov-report=term-missing`. See Appendix C.

## 4. PATH COVERAGE: AccountsFile.load
**Why this method:** `load` has a `for` loop with three outcomes on each pass (blank line → `continue`, `END` line → `break`, otherwise read the user), and it can also exit with an exception. One file containing a blank line, a user line and `END` runs every statement, so statement coverage needs only one test (SC-ACC-02). Path coverage needs every way through the loop, including running it 0, 1 and 2 or more times.

**Control flow**
```
open file ──(error)──────────────────────────────► exception exit   (PC-09)
   │
   ▼
next line? ──no──────────────────────────────────► return
   │yes                                                ▲
   ▼                                                   │
blank? ──yes──► continue ──────────────────────────────┤ (back to "next line?")
   │no                                                 │
   ▼                                                   │
END? ──yes──► break ──► return                         │
   │no                                                 │
   ▼                                                   │
parse fields ──(bad credit)──► exception exit (PC-10)  │
   │ok                                                 │
   └──► store user ────────────────────────────────────┘
```

| Test ID | File content | Path through `load` | Result |
|---|---|---|---|
| PC-01 | (empty) | loop runs 0 times → return | Pass |
| PC-02 | END | END → break | Pass |
| PC-03 | blank | blank → continue → end of file (no END) | Pass |
| PC-04 | user | user → store → end of file (no END) | Pass |
| PC-05 | blank, END | blank → continue → END → break | Pass |
| PC-06 | user, END | user → store → END → break | Pass |
| PC-07 | user, user, END, extra line | user → store → user → store → END → break (extra line ignored) | Pass |
| PC-08 | blank, user, END | blank → continue → user → store → END → break | Pass |
| PC-09 | (missing file) | open fails → exception | Pass |
| PC-10 | user with bad credit, END | parse fails → ValueError | Pass |

**Why these paths are complete:** a loop has unlimited paths in theory (one for every possible number of lines), so the loop was limited in the usual way. It is tested 0 times (PC-01), once (PC-02 to PC-04), and twice or more (PC-05 to PC-08). Every outcome of a single pass (blank → continue, END → break, user → store, bad line → exception) appears at least once. The two outcomes that keep the loop going (blank and user) are each tested both as the last line read (PC-03, PC-04) and followed by another line (PC-05, PC-06, PC-07, PC-08). Both ways of leaving the loop are covered (END → break, and end of file with no END), as are both exception exits (PC-09, PC-10). Running the loop more times would only repeat these same outcomes, so it would add no new path.

Branch coverage of `accounts_file.py` (`--cov-branch`) is also 100%.

## 5. Failures Found and Fixed
All three failures came from `TransactionProcessor` (`backend/transaction_processor.py`). The code had no check for a missing user or for the maximum credit of $999,999.00, so a bad transaction crashed the whole run or left an invalid balance.

| Test ID | Method | Failure observed | Responsible code | Fix applied | Re-run |
|---|---|---|---|---|---|
| SC-PRC-07 | `_delete` | Deleting a user that does not exist raised `KeyError: 'ghost'` and stopped the Back End | `transaction_processor.py` line 82: `self._accounts.remove_user(transaction.username)`: no check that the user exists | Check `self._accounts.exists(...)` first. If the user is missing, log an `UnknownUser` constraint error and skip the transaction | Pass |
| SC-PRC-15 | `_add_credit` | Adding credit to a user that does not exist raised `KeyError: 'ghost'` and stopped the Back End | `transaction_processor.py` line 148: `self._accounts.change_credit(transaction.username, ...)`: no check that the user exists | Same check as above: log `UnknownUser` and skip | Pass |
| SC-PRC-16 | `_add_credit` | A balance could go above $999,999.00. No error was logged and the credit was added (assertion failed: expected `MaxCredit`, output was empty) | `transaction_processor.py` `_add_credit` (line 148): no maximum-credit check before `change_credit` | Added `MAX_CREDIT_CENTS = 99_999_900`. If the new balance would be higher, log a `MaxCredit` constraint error and skip | Pass |

**Evidence (PyCharm pytest runs):**
- **Appendix A: failed-test-run.pdf.** The run before the fixes: 3 failed, 74 passed, exit code 1. It shows the `KeyError: 'ghost'` tracebacks for SC-PRC-07 and SC-PRC-15 and the failed `MaxCredit` assertion for SC-PRC-16.
- **Appendix B: success-run.pdf.** The run after the fixes: 77 passed, exit code 0.

## Appendices
- **Appendix A:** failed-test-run.pdf (PyCharm output, before fixes)
- **Appendix B:** success-run.pdf (PyCharm output, after fixes)
- **Appendix C:** coverage report screenshot (pytest-cov, run in the PyCharm terminal)
