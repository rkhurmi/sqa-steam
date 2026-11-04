#!/usr/bin/env bash
#
# Compares the actual output of a test run against the expected output.
#   - terminal log:     actual/<run>/<category>/<test>_output.txt
#   - transaction file: actual/<run>/<category>/transactions/<test>_actual_transaction.txt
# against the matching files in <category>/expected/. 
# A test with no actual file is reported as MISSING. 
#
# Usage:
#   ./compare_outputs.sh <run>                 compare everything in actual/<run>
#   ./compare_outputs.sh -v <run>              also show the diff for each failure
#   ./compare_outputs.sh -c buy <run>          only the "buy" category
#   ./compare_outputs.sh -o report.txt <run>   save the report to a different file
#
# Run it with no name to list the available run folders.
# The report is printed and saved to reports/<run>_summary.txt.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTS_DIR="$SCRIPT_DIR/../frontend_transactions"
ACTUAL_DIR="$SCRIPT_DIR/../actual"
REPORTS_DIR="$SCRIPT_DIR/../reports"
REPORT_FILE=""

verbose=0
only_category=""
while getopts "vc:o:" opt; do
    case "$opt" in
        v) verbose=1 ;;
        c) only_category="$OPTARG" ;;
        o) REPORT_FILE="$OPTARG" ;;
        *) echo "Usage: $0 [-v] [-c category] [-o report_file] <run>"; exit 2 ;;
    esac
done
shift $((OPTIND - 1))

run_name="${1:-}"

pass=0
fail=0
missing=0
failed_names=()

# Strips carriage returns, trailing whitespace, and trailing blank lines.
normalize() {
    sed -e 's/\r$//' -e 's/[[:space:]]*$//' "$1" | sed -e :a -e '/^\n*$/{$d;N;ba' -e '}'
}

# compare_one <label> <expected file> <actual file>
compare_one() {
    local label="$1" expected="$2" actual="$3"

    if [ ! -f "$actual" ]; then
        echo "MISSING  $label  (no $actual)"
        missing=$((missing + 1))
        failed_names+=("$label (missing)")
        return
    fi

    if diff <(normalize "$expected") <(normalize "$actual") > /dev/null; then
        echo "PASS     $label"
        pass=$((pass + 1))
    else
        echo "FAIL     $label"
        fail=$((fail + 1))
        failed_names+=("$label")
        if [ "$verbose" -eq 1 ]; then
            diff <(normalize "$expected") <(normalize "$actual") | sed 's/^/             /'
        fi
    fi
}

# The run folder must be named; list the choices if it was not.
if [ -z "$run_name" ]; then
    echo "Usage: $0 [-v] [-c category] [-o report_file] <run>"
    echo "Available runs in actual/:"
    for dir in "$ACTUAL_DIR"/*/; do
        [ -d "$dir" ] && echo "  $(basename "$dir")"
    done
    exit 2
fi

OUTPUT_DIR="$ACTUAL_DIR/$run_name"
if [ ! -d "$OUTPUT_DIR" ]; then
    echo "No run folder named '$run_name' in $ACTUAL_DIR"
    echo "Available runs in actual/:"
    for dir in "$ACTUAL_DIR"/*/; do
        [ -d "$dir" ] && echo "  $(basename "$dir")"
    done
    exit 1
fi
[ -n "$REPORT_FILE" ] || REPORT_FILE="$REPORTS_DIR/${run_name}_summary.txt"
mkdir -p "$(dirname "$REPORT_FILE")"

# Everything inside the braces is shown on screen and saved to the report file.
{
echo "Comparison run: $(date)"
echo

for category_dir in "$TESTS_DIR"/*/; do
    category="$(basename "$category_dir")"
    if [ -n "$only_category" ] && [ "$category" != "$only_category" ]; then
        continue
    fi

    # Terminal output: <test>_expected.txt
    for expected in "$category_dir"expected/*_expected.txt; do
        [ -f "$expected" ] || continue
        test_name="$(basename "$expected" _expected.txt)"
        compare_one "$category/$test_name [terminal]" "$expected" "$OUTPUT_DIR/$category/${test_name}_output.txt"
    done

    # Transaction file: <test>_expected_transaction.txt (one is spelled ..._transactions.txt)
    for expected in "$category_dir"expected/transactions/*_expected_transaction*.txt; do
        [ -f "$expected" ] || continue
        test_name="$(basename "$expected" | sed -E 's/_expected_transactions?\.txt$//')"
        compare_one "$category/$test_name [transaction]" "$expected" "$OUTPUT_DIR/$category/transactions/${test_name}_actual_transaction.txt"
    done
done

total=$((pass + fail + missing))
echo
echo "================ SUMMARY ================"
echo "Total: $total   Passed: $pass   Failed: $fail   Missing: $missing"
if [ "${#failed_names[@]}" -gt 0 ]; then
    echo
    echo "Needs attention:"
    for name in "${failed_names[@]}"; do
        echo "  - $name"
    done
fi

[ "$fail" -eq 0 ] && [ "$missing" -eq 0 ]
} | tee "$REPORT_FILE"

# Exit status of the comparison block (not of tee).
exit "${PIPESTATUS[0]}"
