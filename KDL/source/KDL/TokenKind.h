#pragma once

#include <KDL/API.h>
#include <QString>

namespace KDL {

    enum class KDL_API TokenKind
    {
        Unknown,
        Error
    };

    KDL_API [[nodiscard]] QString Stringify(TokenKind kind);
}
