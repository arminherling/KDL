#include <KDL/Lexer.h>

namespace KDL 
{
    TokenBuffer Lex(const QString& source) noexcept
    {
        TokenBuffer buffer{ source };

        return buffer;
    }
}
