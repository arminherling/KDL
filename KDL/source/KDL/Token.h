#pragma once

#include <KDL/API.h>
#include <KDL/TokenKind.h>

namespace  KDL 
{
    struct KDL_API Token
    {
        TokenKind kind = TokenKind::Unknown;
        QStringView stringView;
    };
}
