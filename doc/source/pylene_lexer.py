from pygments.lexers.compiled import CppLexer
from pygments.token import Name, Keyword
import pygments.lexers

class PyleneLexer(CppLexer):
    name = 'PyleneCpp'
    aliases = ['pylenecpp']

    EXTRA_KEYWORDS = ['mln_foreach', 'mln_forall']

    def get_tokens_unprocessed(self, text):
        for index, token, value in CppLexer.get_tokens_unprocessed(self, text):
            if token is Name and value in self.EXTRA_KEYWORDS:
                yield index, Keyword, value
            else:
                yield index, token, value


old = pygments.lexers.get_lexer_by_name

def newf(alias, **options):
    if alias in PyleneLexer.aliases:
        return PyleneLexer(**options)
    else:
        return old(alias, **options)

pygments.lexers.get_lexer_by_name = newf
del newf
