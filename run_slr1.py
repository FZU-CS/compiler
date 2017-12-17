#!/usr/bin/env python

"""Writen by Wasdns(XiangChen), 2017.12.17"""

"""Compiler Tasks: The SLR(1) Analyzer for Pascal language"""

# This project refers to the following materials:
#
# [1]Wikipedia: https://en.wikipedia.org/wiki/Pascal_%28programming_language%29
# [2]Pascal-Interpreter: https://github.com/kevallakhani95/Pascal-Interpreter/
# [3]P4-HLIR: https://github.com/p4lang/p4-hlir

import sys
from collections import OrderedDict

from tockenizer import Lexer

def main():
	text = open(sys.argv[1], 'rU').read()
	lexer = Lexer(text)

	current_token = lexer.get_next_token()
	while current_token is not None:
		if current_token.type == 'EOF':
			#print lexer.current_line
			break
		current_token = lexer.get_next_token()
		print current_token

	

if __name__ == '__main__':
	main()