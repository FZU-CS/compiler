set -e

# Create build_parser folder
sudo ./cleanup.sh
mkdir build_parser

# Using lexer to get tockens
# ./run_lexer.py parser_tests/given_example.pas > build_parser/tocken.txt

# TBD: tocken.txt to input.txt

# Compile parser, written in C++
gcc src/parser.cpp -Wall -ljsoncpp -lstdc++ -o build_parser/parser

# Touch new output files
cp -r parser_tests/standard_input.txt build_parser/input.txt
cp -r docs/grammar/parser_tasks/parsed_grammar.txt build_parser/grammar.txt
touch build_parser/output.txt
touch build_parser/slr.txt
touch build_parser/error.txt
touch build_parser/action_and_goto.txt

# Parsing
cd build_parser/
./parser > first_and_follow.txt
