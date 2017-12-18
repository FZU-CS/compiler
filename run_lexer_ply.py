#!/usr/bin/env python
import os
import argparse
from ply_frontend.ply_lexer import *
from ply_frontend.preprocessor import Preprocessor, PreprocessorException

parser = argparse.ArgumentParser(description='Simple Pascal program lexer')
parser.add_argument('--program-name', help='The name of input program',
					type=str, action="store", required=True)
args = parser.parse_args()

def main():
	# Input program name
	program_name = args.program_name

	source_files = []
	source_files.append(program_name)

	source_txt = []

	# Preprocess all program text
	preprocessed_sources = []
	try:
		preprocessor = Preprocessor()
		preprocessor.args += ["-E", "-x", "c", "-w"]

		for source in source_files:
			# Join the work directory to absolute sources
			absolute_source = os.path.join(os.getcwd(), source)

			if not os.path.isfile(absolute_source):
				print "Source file '" + source + "' could not be opened or does not exist."
				return False

			preprocessed_sources.append(preprocessor.preprocess_file(
				absolute_source,
				# No need for dump preprocessed
				#dest='%s.i'%source if dump_preprocessed else None
				dest=None
			))

		for txt in source_txt:
			preprocessed_sources.append(preprocessor.preprocess_str(
				txt,
				dest=None
			))

	except PreprocessorException as e:
		print str(e)
		return False

	# Lexer Test
	for preprocessed_source in preprocessed_sources:
		lexer = Lexer()
		lexer.build()
		lexer.reset_lineno()
		lexer.test(preprocessed_source)
	print "\ntesting successful\n"

	"""
	# Parse preprocessed text
	all_p4_objects = []
	for preprocessed_source in preprocessed_sources:
		p4_objects, errors_cnt = P4Parser().parse(preprocessed_source)
		if errors_cnt > 0:
			print errors_cnt, "errors during parsing"
			print "Interrupting compilation"
			return False
		all_p4_objects += p4_objects
	print "parsing successful"
	"""

if __name__ == '__main__':
	main()
