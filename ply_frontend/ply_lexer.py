from ply import lex
from ply.lex import TOKEN
import re

class Lexer:
    def __init__(self):
        self.filename = ''
        # Keeps track of the last token returned from self.token()
        self.last_token = None
        # Allow either "# line" or "# <num>" to support GCC's cpp output
        self.line_pattern = re.compile('([ \t]*line\W)|([ \t]*\d+)')
        self.errors_cnt = 0

    def reset_lineno(self):
        """ Resets the internal line number counter of the lexer.
        """
        self.lexer.lineno = 1

    def get_lineno(self):
        return self.lexer.lineno

    # input() and token() are required when building parser from this lexer
    def input(self, text):
        self.lexer.input(text)

    def token(self):
        self.last_token = self.lexer.token()
        # print self.last_token
        return self.last_token

    def find_tok_column(self, token):
        """ Find the column of the token in its line.
        """
        last_cr = self.lexer.lexdata.rfind('\n', 0, token.lexpos)
        return token.lexpos - last_cr

    def _error(self, s, token):
        print s, "in file", self.filename, "at line", self.get_lineno()
        self.errors_cnt += 1

    keywords = (
        'SUB',
        'PROGRAM',
        'VAR',
        'DIV',
        'INTEGER',
        'REAL',
        'BEGIN',
        'END',
        'PROCEDURE',
    )

    keywords_map = {}
    for keyword in keywords:    
        keywords_map[keyword.lower()] = keyword

    tokens = (
        # identifiers
        'ID',
        
        # constants
        'INT_CONST_DEC', 'INT_CONST_HEX',

        # operators
        'PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'MOD',
        'OR', 'AND', 'NOT', 'XOR', 'LSHIFT', 'RSHIFT',
        'LOR', 'LAND', 'LNOT', # see above
        'LT', 'LE', 'GT', 'GE', 'EQ', 'NE',
        
        # Delimeters
        'LPAREN', 'RPAREN', # ( )
        'LBRACKET', 'RBRACKET', # [ ]
        'LBRACE', 'RBRACE', # { }
        'COMMA', 'PERIOD', # . ,
        'SEMI', 'COLON', # ; :
        'APOSTROPHE', # '

        'PRAGMA', 'STR',
    ) + keywords
    
    # Regular expression rules for simple tokens
    t_PLUS    = r'\+'
    t_MINUS   = r'-'
    t_TIMES   = r'\*'
    t_DIVIDE  = r'/'
    t_MOD = r'%'
    t_OR = r'\|'
    t_AND = r'&'
    t_NOT = r'~'
    t_XOR = r'\^'
    t_LSHIFT = r'<<'
    t_RSHIFT = r'>>'
    t_LT = r'<'
    t_GT = r'>'
    t_LE = r'<='
    t_GE = r'>='
    t_EQ = r':=' # different from P4 language
    t_NE = r'!='

    t_LPAREN  = r'\('
    t_RPAREN  = r'\)'
    t_LBRACKET  = r'\['
    t_RBRACKET  = r'\]'
    t_LBRACE  = r'\{'
    t_RBRACE  = r'\}'
    t_COMMA = r','
    t_PERIOD = r'\.'
    t_SEMI = r';'
    t_COLON = r':'
    t_APOSTROPHE = r'\''

    # valid C identifiers (K&R2: A.2.3), plus '$' (supported by some compilers)
    identifier = r'[a-zA-Z_$][0-9a-zA-Z_$]*'
    hex_prefix = '0[xX]'
    hex_digits = '[0-9a-fA-F]+'
    # integer constants (K&R2: A.2.5.1)
    decimal_constant = '[0-9]+'
    hex_constant = hex_prefix + hex_digits

    simple_escape = r"""([a-zA-Z._~!=&\^\-\\?'"])"""
    decimal_escape = r"""(\d+)"""
    hex_escape = r"""(x[0-9a-fA-F]+)"""
    bad_escape = r"""([\\][^a-zA-Z._~^!=&\^\-\\?'"x0-7])"""
    escape_sequence = r"""(\\("""+simple_escape+'|'+decimal_escape+'|'+hex_escape+'))'
    string_char = r"""([^"\\\n]|"""+escape_sequence+')'
    string_literal = '"'+string_char+'*"'

    def t_PRAGMA(self, t):
        r'@pragma'
        t.lexer.begin('pragma')
        return t

    @TOKEN(identifier)
    def t_ID(self, t):
        t.type = self.keywords_map.get(t.value, "ID")
        return t

    @TOKEN(hex_constant)
    def t_INT_CONST_HEX(self, t):
        return t

    @TOKEN(decimal_constant)
    def t_INT_CONST_DEC(self, t):
        return t

    ##
    ## Lexer states: used for preprocessor \n-terminated directives
    ##
    states = (
        # ppline: preprocessor line directives
        #
        ('ppline', 'exclusive'),
        ('pragma', 'exclusive'),
    )

    def t_PPHASH(self, t):
        r'\#'
        if self.line_pattern.match(t.lexer.lexdata, pos=t.lexer.lexpos):
            t.lexer.begin('ppline')
            self.pp_line = self.pp_filename = None
        else:
            self._error("unexpected '#' character", t)
            # skip rest of line...
            line_start = self.lexer.lexdata.rfind('\n', 0, t.lexpos) + 1
            line_end = self.lexer.lexdata.find('\n', t.lexpos, -1)
            t.lexer.skip(line_end - line_start)

    def t_pragma_NEWLINE(self, t):
        r'\n'
        t.lexer.lineno += t.value.count("\n")
        t.lexer.begin('INITIAL')

    def t_pragma_STR(self, t):
        r'.+'
        return t
        
    def t_pragma_error(self, t):
        self._error('invalid pragma', t)

    ##
    ## Rules for the ppline state
    ##
    @TOKEN(string_literal)
    def t_ppline_FILENAME(self, t):
        if self.pp_line is None:
            self._error('filename before line number in #line', t)
        else:
            self.pp_filename = t.value.lstrip('"').rstrip('"')

    @TOKEN(decimal_constant)
    def t_ppline_LINE_NUMBER(self, t):
        if self.pp_line is None:
            self.pp_line = t.value
        else:
            # Ignore: GCC's cpp sometimes inserts a numeric flag
            # after the file name
            pass

    def t_ppline_NEWLINE(self, t):
        r'\n'
        if self.pp_line is None:
            self._error('line number missing in #line', t)
        else:
            self.lexer.lineno = int(self.pp_line)

            if self.pp_filename is not None:
                self.filename = self.pp_filename

        t.lexer.begin('INITIAL')

    def t_ppline_PPLINE(self, t):
        r'line'
        pass

    t_ppline_ignore = ' \t'

    t_pragma_ignore = ' \t'

    ##
    ## Rules for the normal state
    ##
    t_ignore = ' \t'

    # Newlines
    def t_NEWLINE(self, t):
        r'\n+'
        t.lexer.lineno += t.value.count("\n")

    def t_ppline_error(self, t):
        self._error('invalid #line directive', t)

    # Error handling rule
    def t_error(self,t):
        self._error("illegal character '%s'" % t.value[0], t)
        t.lexer.skip(1)
    
    # Build the lexer
    def build(self,**kwargs):
        self.lexer = lex.lex(module=self, **kwargs)
    
    # Test it output
    def test(self,data):
        self.lexer.input(data)
        while True:
             tok = self.lexer.token()
             if not tok: break
             print tok
