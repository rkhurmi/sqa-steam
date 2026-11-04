#!/bin/bash
#
# run_tests.sh
#
# Builds the distribution-system program, then runs every test input
# through it and saves two actual outputs per test:
#   - actual/<transaction_type>/<test_name>_output.txt             (terminal log)
#   - actual/<transaction_type>/<test_name>_actual_transaction.txt (daily transaction file)
#
# Usage (run from the project root):
#   ./frontend_transaction_tests/scripts/run_tests.sh

set -eu

# ---- find the project folders, no matter where this script is run from ----
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
TESTS_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
PROJECT_DIR=$(cd "$TESTS_DIR/.." && pwd)

CODE_DIR="$PROJECT_DIR/frontend_code"
FIXTURES_DIR="$TESTS_DIR/fixtures"
TRANSACTIONS_DIR="$TESTS_DIR/frontend_transactions"
OUTPUT_DIR="$TESTS_DIR/actual"
PROGRAM="$CODE_DIR/distribution-system"

ACCOUNTS_FILE="$FIXTURES_DIR/currentaccounts.txt"
GAMES_FILE="$FIXTURES_DIR/availablegames.txt"
COLLECTION_FILE="$FIXTURES_DIR/gamescollection.txt"

# ---- step 1: build the program --------------------------------------------
echo "Building $PROGRAM"
g++ -std=c++17 -o "$PROGRAM" "$CODE_DIR"/*.cpp

for fixture_file in "$ACCOUNTS_FILE" "$GAMES_FILE" "$COLLECTION_FILE"; do
    if [ ! -f "$fixture_file" ]; then
        echo "Missing fixture file: $fixture_file" >&2
        exit 1
    fi
done

# ---- step 2: run every test ------------------------------------------------
# Each test is one input file: <transaction_type>/input/<test_name>_input.txt
test_count=0

for transaction_dir in "$TRANSACTIONS_DIR"/*/; do
    transaction_type=$(basename "$transaction_dir")
    input_dir="$transaction_dir/input"
    [ -d "$input_dir" ] || continue

    test_output_dir="$OUTPUT_DIR/$transaction_type"
    mkdir -p "$test_output_dir"

    for input_file in "$input_dir"/*_input.txt; do
        [ -e "$input_file" ] || continue

        input_file_name=$(basename "$input_file")
        test_name=${input_file_name%_input.txt}

        test_count=$((test_count + 1))
        echo "Running Test $test_count: $test_name"

        output_file="$test_output_dir/${test_name}_output.txt"
        transaction_file="$test_output_dir/${test_name}_actual_transaction.txt"

        # delete any transaction file left over from a previous run of this test
        rm -f "$transaction_file"

        "$PROGRAM" "$ACCOUNTS_FILE" "$GAMES_FILE" "$COLLECTION_FILE" "$transaction_file" \
            < "$input_file" > "$output_file"

        # the program only creates the transaction file if it writes something to
        # it, so create an empty one here when a test ends with nothing recorded
        [ -f "$transaction_file" ] || : > "$transaction_file"
    done
done

echo "Ran $test_count test(s)"
