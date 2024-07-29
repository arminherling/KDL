#pragma once

#include <KDL/API.h>
#include <QString>

namespace KDL {

    enum class KDL_API TokenKind
    {
        Unknown,
        Error,
        Equal,
        OpenParenthesis,
        CloseParenthesis,
        OpenBracket,
        CloseBracket,
        Newline,
        Terminator,
        Keyword_True,
        Keyword_False,
        Keyword_NaN,
        Keyword_Infinity,
        Keyword_NegativeInfinity,
        EndOfFile
    };

    KDL_API [[nodiscard]] QString Stringify(TokenKind kind);
}
