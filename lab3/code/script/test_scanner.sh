#!/bin/bash

# Directory containing test files
TEST_DIR="../test"
# Directory to store output files
OUTPUT_DIR="../output/scanner"

# Create output directory if it doesn't exist
mkdir -p $OUTPUT_DIR

# Loop through all test files in the test directory
for test_file in $TEST_DIR/*; do
    # Get the base name of the test file
    base_name=$(basename $test_file)
    # Run the scanner and redirect the output to the output directory
    ./scanner $test_file > $OUTPUT_DIR/${base_name}.out 2>&1
done
