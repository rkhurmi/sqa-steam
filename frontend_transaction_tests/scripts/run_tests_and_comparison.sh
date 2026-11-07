#!/bin/bash
#
# run_tests_and_comparison.sh
#
# Runs the whole test process with one command:
#   1. run_tests.sh       - builds the Front End and runs every test input,
#                           saving each test's terminal log and transaction file
#   2. compare_outputs.sh - compares those outputs with the expected outputs,
#                           shows the diff for each failure, and saves a report
#
# Usage (run from the project root):
#   ./frontend_transaction_tests/scripts/run_tests_and_comparison.sh            run named after the date and time
#   ./frontend_transaction_tests/scripts/run_tests_and_comparison.sh my_run     run named "my_run"
#
# Results:  frontend_transaction_tests/actual/<run_name>/
# Report:   frontend_transaction_tests/reports/<run_name>_summary.txt
# Exits with 0 only when every test passes.

set -eu

# ---- find the scripts folder, no matter where this is run from ----
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

# ---- the same run name is used by both steps, so the comparison finds the outputs ----
RUN_NAME=${1:-$(date +%Y-%m-%d_%H-%M-%S)}

# ---- step 1: build and run every test ----
"$SCRIPT_DIR/run_tests.sh" "$RUN_NAME"

echo
echo "================ COMPARING OUTPUTS ================"
echo

# ---- step 2: compare actual outputs with expected outputs and print the summary ----
"$SCRIPT_DIR/compare_outputs.sh" -v "$RUN_NAME"
