set -e

# Create build folder
sudo ./cleanup.sh
mkdir build_ir

# Using lexer to get tockens
# ./run_lexer.py parser_tests/given_example.pas > build_ir/tocken.txt

# TBD: tocken.txt to input.txt

# Compile ir, written in C++
gcc src/ir.cpp -Wall -ljsoncpp -lstdc++ -o build_ir/ir

# Touch new output files
cp -r ir_tests/$1/input.txt build_ir/input.txt
cp -r ir_tests/$1/top.txt build_ir/top.txt
cp -r docs/grammar/ir_tasks/parsed_grammar.txt build_ir/grammar.txt
touch build_ir/output.txt
touch build_ir/slr.txt
touch build_ir/error.txt
touch build_ir/action_and_goto.txt
touch build_ir/ir_code.txt

# Parsing
cd build_ir/
./ir > first_and_follow.txt
