#!/bin/sh
# Runs every tests/*/*.mc through ./lexify.
#  valid/            must exit 0
#  lexical_errors/   must exit 1 and print "Lexical error"
#  syntax_errors/    must exit 1 and print "Syntax error"
#  semantic_errors/  skipped until the semantic phase exists
#                    (then: must exit 1 and print "Semantic error")
cd "$(dirname "$0")/.." || exit 2
BIN=./lexify
pass=0; fail=0; skip=0

check() {  # dir  expected_exit  expected_text
    for f in tests/$1/*.mc; do
        out=$($BIN "$f" 2>&1); code=$?
        if [ "$code" -eq "$2" ] && { [ -z "$3" ] || echo "$out" | grep -q "$3"; }; then
            echo "PASS  $f"; pass=$((pass+1))
        else
            echo "FAIL  $f"; echo "$out" | sed 's/^/      /'; fail=$((fail+1))
        fi
    done
}

check valid 0 ""
check lexical_errors 1 "Lexical error"
check syntax_errors 1 "Syntax error"
for f in tests/semantic_errors/*.mc; do
    if [ -f tests/.semantic_ready ]; then
        out=$($BIN "$f" 2>&1); code=$?
        if [ "$code" -eq 1 ] && echo "$out" | grep -q "Semantic error"; then
            echo "PASS  $f"; pass=$((pass+1))
        else echo "FAIL  $f"; fail=$((fail+1)); fi
    else
        echo "SKIP  $f (semantic phase not implemented; create tests/.semantic_ready to enable)"; skip=$((skip+1))
    fi
done
echo; echo "passed=$pass failed=$fail skipped=$skip"
[ "$fail" -eq 0 ]
