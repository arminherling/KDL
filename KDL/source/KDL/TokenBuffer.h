#pragma once

#include <KDL/API.h>
#include <KDL/TokenKind.h>
#include <KDL/Token.h>
#include <Defines.h>

#include <vector>

namespace KDL
{
    class KDL_API TokenBuffer
    {
    public:
        TokenBuffer(const QString& source);

        void addToken(TokenKind kind, i32 start, i32 end) noexcept;

        [[nodiscard]] i32 size() const noexcept;
        [[nodiscard]] Token operator[](i32 index) const noexcept;

    private:
        QString m_source;
        std::vector<TokenKind> m_tokenKinds;
        std::vector<i32> m_startIndexes;
        std::vector<i32> m_endIndexes;
    };
}
