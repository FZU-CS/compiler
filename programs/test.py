#!/usr/bin/env python
import os
import sys
import commands

# the index of lexer tests
lexer_test_idx = 1

def list_pas_programs(pas_programs_dir):
    """List Pascal programs"""
    files = os.listdir(pas_programs_dir)
    pas_programs = []
    for f in files:
    	if os.path.splitext(f)[1] == '.pas':
    		pas_programs.append(f) 
    return pas_programs

def test_lexer(pas_programs_dir):
    """Test lexer with pascal scripts"""
    global lexer_test_idx

    programs = list_pas_programs(pas_programs_dir)
    total_tests = len(programs)

    status = 0

    for p in programs:
        # Print test information:
        print "===Test%d: %s===" % (lexer_test_idx, p)

        # Execute tests
        cmd = "python ../run.py %s%s" % (pas_programs_dir, p)
        status, output = commands.getstatusoutput(cmd)
        if status != 0:
            print output
            print "\nError occured in file %s\n" % p 
            break

        print " status: passed"
        lexer_test_idx += 1

    return status

def main():
    test_dir = ['./', 'pascalscripts/']

    for directory in test_dir:
        status = test_lexer(directory)
        if status != 0:
            print "\nError: Something goes wrong!\n"
            return
    
    print "\nCongratulations: All the tests of lexer have been passed! Total: %d tests.\n" % lexer_test_idx

if __name__ == '__main__':
    main()