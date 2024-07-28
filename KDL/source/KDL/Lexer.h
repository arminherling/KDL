#pragma once

#include <KDL/API.h>
#include <KDL/TokenBuffer.h>
#include <QString>

namespace KDL
{
    KDL_API [[nodiscard]] TokenBuffer Lex(const QString& source) noexcept;
}
