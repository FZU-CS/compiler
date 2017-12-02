#!/usr/bin/env python

"""Writen by Wasdns(XiangChen), 2017.12.2"""

"""Compiler Tasks: The Lexer for Pascal language"""

import sys
from collections import OrderedDict

# Hex List
HEX_LIST = ['A', 'B', 'C', 'D', 'E', 'F', 
			'a', 'b', 'c', 'd', 'e', 'f']

MAXINT = 2147483647

# Token types

INTEGER       = 'INTEGER'
REAL          = 'REAL'
INTEGER_CONST = 'INTEGER_CONST'
REAL_CONST    = 'REAL_CONST'
PLUS          = 'PLUS'
MINUS         = 'MINUS'
MUL           = 'MUL'
INTEGER_DIV   = 'INTEGER_DIV'
FLOAT_DIV     = 'FLOAT_DIV'
LPAREN        = 'LPAREN'
RPAREN        = 'RPAREN'
ID            = 'ID'
ASSIGN        = 'ASSIGN'
BEGIN         = 'BEGIN'
END           = 'END'
SEMI          = 'SEMI'
DOT           = 'DOT'
PROGRAM       = 'PROGRAM'
VAR           = 'VAR'
COLON         = 'COLON'
COMMA         = 'COMMA'
# EOF (End-Of-File) token is used to indicate that
# there is no more input left for lexical analysis
EOF           = 'EOF'
PROCEDURE	  = 'PROCEDURE'

class Token(object):
	def __init__(self, type, value):
		self.type = type
		self.value = value

	def __str__(self):
		return 'Token({type}, {value})'.format(
			type = self.type,
			value = repr(self.value)
		)

	def __repr__(self):
		return self.__str__()

RESERVED_KEYWORDS = {
	'PROGRAM': Token('PROGRAM', 'PROGRAM'),
	'VAR': Token('VAR', 'VAR'),
	'DIV': Token('INTEGER_DIV', 'DIV'),
	'INTEGER': Token('INTEGER', 'INTEGER'),
	'REAL': Token('REAL', 'REAL'),
	'BEGIN': Token('BEGIN', 'BEGIN'),
	'END': Token('END', 'END'),
	'PROCEDURE': Token('PROCEDURE', 'PROCEDURE'), 
}

class Lexer(object):
	"""A Lexer for Pascal Language"""
	def __init__(self,text):
		self.text = text
		self.pos = 0
		self.current_char = self.text[self.pos]

	"""Exception Handling"""

	def error(self):
		raise Exception('Invalid character')

	def bin_error(self):
		"""Instead of raising error, we choose to print the bin error"""
		print "Bin Number Error"

	def oct_error(self):
		"""Instead of raising error, we choose to print the oct error"""
		print "Oct Number Error"

	def hex_error(self):
		"""Instead of raising error, we choose to print the hex error"""
		print "Hex Number Error"

	def number_error(self):
		"""Get a number with errors"""
		print "Number Organization Error"
		while self.current_char is not None and self.current_char != ";":
			self.advance()

	def print_blank_info(self):
		"""Print Something as Error Occurrs"""
		print "WARNING: Cannot acquire Token because of lexer error!\n"

	"""Basic Functions"""

	def advance(self):
		"""Move to Next Char Location"""
		self.pos += 1
		if self.pos > len(self.text)-1:
			self.current_char = None   		# Indicates end of input
		else:
			self.current_char = self.text[self.pos]

	def look_ahead(self, pos):
		"""Look up Next Char"""
		next_char = None
		next_pos = pos+1
		if next_pos > len(self.text)-1:
			return next_char
		else:
			next_char = self.text[next_pos]
			return next_char

	def acquire_char(self, pos):
		"""Acquire Char Based on Location"""
		if pos > len(self.text)-1:
			return None # Indicates end of input
		else:
			return self.text[pos]
	
	def skip_comment(self):
		"""Skip Pascal Comments"""
		while self.current_char != '}':
			self.advance()
		self.advance() # Skips until the closing curly brace
		# print "skip_comment\n"

	def skip_whitespace(self):
		"""Skip White Spaces"""
		while self.current_char is not None and self.current_char.isspace():
			self.advance()
		# print "skip_whitespace\n"

	def peek(self):
		"""Another Look Ahead Function"""
		peek_pos = self.pos + 1
		if peek_pos > len(self.text) - 1:
			return None
		else:
			return self.text[peek_pos]

	def _id(self):
		"""Handle Identifiers and Reserved Keywords"""
		result = ''

		while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '_'):
			result += self.current_char
			self.advance()

		token = RESERVED_KEYWORDS.get(result, Token(ID, result))
		return token

	"""Handling Numbers"""

	def dec_number(self):
		"""Acquire Normal Number"""
		result = ''
		while self.current_char is not None and self.current_char.isdigit():
			result += self.current_char
			self.advance()

		if self.current_char == '.':
			result += self.current_char
			self.advance()

			while self.current_char is not None and self.current_char.isdigit():
				result += self.current_char
				self.advance()

			token = Token('REAL_CONST', float(result))

		else:
			if int(result) > MAXINT:
				print "Error: The integer is exceeded!"
			token = Token('INTEGER_CONST', int(result))
		
		print "Get a dec_number", result

		return token

	def bin_number(self):
		"""Acquire Bin. Number, Report Error"""
		result = ''
		while self.current_char is not None and self.current_char.isdigit():
			result += self.current_char
			self.advance()

		if self.current_char == '.':
			result += self.current_char
			self.advance()

			while self.current_char is not None and self.current_char.isdigit():
				result += self.current_char
				self.advance()

			# token = Token('REAL_CONST', float(result))

		else:
			pass
			# token = Token('INTEGER_CONST', int(result))
		
		print "Get a bin_number", result

		# We do not regard this value as TOKEN, simply report Error
		return self.bin_error()
		#return token

	def oct_number(self):
		"""Acquire Oct. Number, Report Error"""
		result = ''
		while self.current_char is not None and self.current_char.isdigit():
			result += self.current_char
			self.advance()

		if self.current_char == '.':
			result += self.current_char
			self.advance()

			while self.current_char is not None and self.current_char.isdigit():
				result += self.current_char
				self.advance()

			# token = Token('REAL_CONST', float(result))

		else:
			pass
			# token = Token('INTEGER_CONST', int(result))
		
		print "Get a oct_number", result

		# We do not regard this value as TOKEN, simply report Error
		return self.oct_error()
		#return token

	def hex_number(self):
		"""Acquire Hex. Number, Report Error"""
		result = ''
		while True:
			if self.current_char is not None and self.current_char.isdigit():
				result += self.current_char
				self.advance()
			elif self.current_char is not None and self.current_char == 'x':
				result += self.current_char
				self.advance()
			elif self.current_char is not None and self.current_char == 'X':
				result += self.current_char
				self.advance()
			elif self.current_char is not None and self.current_char in HEX_LIST:
				result += self.current_char
				self.advance()
			else:
				break

		if self.current_char == '.':
			result += self.current_char
			self.advance()

			while True:
				if self.current_char is not None and self.current_char.isdigit():
					result += self.current_char
					self.advance()
				elif self.current_char is not None and self.current_char == 'x':
					result += self.current_char
					self.advance()
				elif self.current_char is not None and self.current_char == 'X':
					result += self.current_char
					self.advance()
				elif self.current_char is not None and self.current_char in HEX_LIST:
					result += self.current_char
					self.advance()
				else:
					break

			# token = Token('REAL_CONST', float(result))

		else:
			pass
			# token = Token('INTEGER_CONST', int(result))
		
		print "Get a hex_number", result

		# We do not regard this value as TOKEN, simply report Error
		return self.hex_error()
		#return token

	"""Number Type Judger"""

	def judge_type(self, start_pos):
		"""Return type flag in terms of an integer
		   1:bin, 2:oct, 3:dec 4:hex"""
		result = -1
		isSkipPoint = False
		
		# Current Position
		current_pos = start_pos
		while True: 
			if self.look_ahead(current_pos).isdigit() or self.look_ahead(current_pos) in HEX_LIST:
				# is Digit? 0-9
				if self.look_ahead(current_pos).isdigit():
					int_number = int(self.look_ahead(current_pos))
					if int_number < 2 and result == -1: # bin
						result = 1
					elif int_number < 8 and int_number > 1 and result < 2: # oct
						result = 2
					elif int_number < 10 and int_number > 7 and result < 3: # dec
						result = 3
				else: # hex
					result = 4
			else:
				# If reach ';', break
				if self.look_ahead(current_pos) == ';':
					break
				# If meet '.'
				elif self.look_ahead(current_pos) == '.':
					if not isSkipPoint:
						isSkipPoint = True
					else: # The procedure has met '.'
						return None
				else: # Something is wrong
					return None
			current_pos += 1

		return result

	"""Lexer: Acquiring Tokens"""

	def get_next_token(self):
		"""Lexical analyzer (also known as scanner or tokenizer)
        This method is responsible for breaking a sentence
        apart into tokens. One token at a time.
        """
		while self.current_char is not None:
			
			# White Space
			if self.current_char.isspace():
				self.skip_whitespace()
				continue

			# Character
			if self.current_char.isalpha():
				return self._id()

			# Handling Number
			if self.current_char.isdigit():
				if self.current_char != '0':
					return self.dec_number()
				else:
					current_pos = self.pos
					next_char = self.look_ahead(current_pos)

					if next_char == 'X' or next_char == 'x':
						self.hex_number()
					elif next_char == '0':
						flag = self.judge_type(current_pos+1) # 0[pt.]...
						
						if flag == 1:
							self.bin_number()
						elif flag == 2:
							self.oct_number()
						elif flag == 3:
							# This function has token printed
							return self.dec_number()
						elif flag == 4:
							self.hex_number()
						else: # if the number is wrong, report error
							self.number_error()
					else:
						self.oct_number()
						
				# Cannot return Token, print blank information
				self.print_blank_info()

			if self.current_char == ':' and self.peek() == '=':
				self.advance()
				self.advance()
				return Token(ASSIGN, ':=')

			if self.current_char == ';':
				self.advance()
				return Token(SEMI, ';')

			if self.current_char == '+':
				self.advance()
				return Token(PLUS, '+')

			if self.current_char == '-':
				self.advance()
				return Token(MINUS, '-')

			if self.current_char == '*':
				self.advance()
				return Token(MUL, '*')

			if self.current_char == '/':
				self.advance()
				return Token(FLOAT_DIV, '/')

			if self.current_char == '(':
				self.advance()
				return Token(LPAREN, '(')

			if self.current_char == ')':
				self.advance()
				return Token(RPAREN, ')')

			if self.current_char == '.':
				self.advance()
				return Token(DOT, '.')

			if self.current_char == '{':
				self.advance()
				self.skip_comment()
				continue

			if self.current_char == ':':
				self.advance()
				return Token(COLON, ':')

			if self.current_char == ',':
				self.advance()
				return Token(COMMA, ',')

			self.error()

		return Token(EOF, None)
