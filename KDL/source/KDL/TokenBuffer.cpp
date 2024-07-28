#include <KDL/TokenBuffer.h>

namespace KDL 
{
    TokenBuffer::TokenBuffer(const QString& source)
        : m_source{ source }
        , m_tokenKinds{}
        , m_startIndexes{}
        , m_endIndexes{}
    {
        const auto size = source.size();
        m_tokenKinds.reserve(size);
        m_startIndexes.reserve(size);
        m_endIndexes.reserve(size);
    }

    void TokenBuffer::addToken(TokenKind kind, i32 start, i32 end) noexcept
    {
        m_tokenKinds.push_back(kind);
        m_startIndexes.push_back(start);
        m_endIndexes.push_back(end);
    }

    i32 TokenBuffer::size() const noexcept
    {
        return m_tokenKinds.size();
    }

    Token TokenBuffer::operator[](i32 index) const noexcept
    {
        const auto kind = m_tokenKinds.at(index);
        const auto start = m_startIndexes.at(index);
        const auto end = m_endIndexes.at(index);
        const auto length = end - start;
        const auto stringView = QStringView(m_source).sliced(start, length);

        return { .kind = kind, .stringView = stringView };
    }
}