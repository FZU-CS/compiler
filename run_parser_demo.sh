# Create build folder
sudo ./cleanup.sh
mkdir build

# Using lexer to get tockens
# ./run_lexer.py parser_tests/given_example.pas > build/tocken.txt

# TBD: tocken.txt to input.txt

# Compile parser, written in C++
gcc src/parser.cpp -Wall -ljsoncpp -lstdc++ -o build/parser

# Touch new output files
cp -r parser_tests/standard_input.txt build/input.txt
cp -r docs/grammar/parsed_grammar.txt build/grammar.txt
touch build/output.txt
touch build/slr.txt
touch build/error.txt
touch build/action_and_goto.txt

# Parsing
cd build/
./parser > first_and_follow.txt
