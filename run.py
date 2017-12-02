#!/usr/bin/env python

import sys
from collections import OrderedDict

from tockenizer import Lexer

def main():
	text = open(sys.argv[1], 'r').read()
	lexer = Lexer(text)

	current_token = lexer.get_next_token()
	while current_token is not None:
		if current_token.type == 'EOF':
			break
		current_token = lexer.get_next_token()
		print current_token, '\n'

if __name__ == '__main__':
	main()