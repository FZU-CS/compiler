# Create build folder
mkdir build

# Using lexer to get tockens
./run_lexer.py parser_tests/given_example.pas > build/tocken.txt

# Compile parser, written in C++
gcc src/parser.cpp -Wall -ljsoncpp -lstdc++ -o build/parser

# Touch new output files
touch build/project_set.txt
touch build/action_and_goto.txt

# Parsing
cd build/
./parser
