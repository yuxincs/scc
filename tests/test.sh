#!/bin/bash

TEST_DIR=$(dirname "${BASH_SOURCE[0]}")

$1 ${TEST_DIR}/factory.c -o ./factory.asm
if [[ "$((echo "10" | spim -file ./factory.asm) | grep "3628800")" != "3628800" ]]; then exit $?; fi
$1 ${TEST_DIR}/fibonacci.c -o ./fibonacci.asm
if [[ "$((echo "10" | spim -file ./fibonacci.asm) | grep "55")" != "55" ]]; then exit $?; fi
$1 ${TEST_DIR}/full_syntax_errors.c
$1 ${TEST_DIR}/full_semantic_errors.c
