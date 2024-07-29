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
        Terminator,
        EndOfFile
    };

    KDL_API [[nodiscard]] QString Stringify(TokenKind kind);
}
