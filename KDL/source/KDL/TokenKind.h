#pragma once

#include <KDL/API.h>
#include <QString>

namespace KDL {

    enum class KDL_API TokenKind
    {
        Unknown,
        Error,
        Newline,
        Equal,
        OpenParenthesis,
        CloseParenthesis,
        OpenBracket,
        CloseBracket,
        SlashDash,
        Terminator,
        Number_Binary,
        Number_Octal,
        Number_Hexadecimal,
        Number_Decimal,
        Identifier,
        Keyword_True,
        Keyword_False,
        Keyword_NaN,
        Keyword_Infinity,
        Keyword_NegativeInfinity,
        Keyword_Null,
        EndOfFile
    };

    KDL_API [[nodiscard]] QString Stringify(TokenKind kind);
}
