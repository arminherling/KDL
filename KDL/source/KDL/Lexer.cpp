#include <KDL/Lexer.h>

namespace 
{
    using namespace KDL;

    [[nodiscard]] static auto PeekChar(const QString& source, i32 currentIndex, i32 offset) noexcept
    {
        const auto charIndex = currentIndex + offset;
        if (charIndex >= source.length())
            return QChar('\0');

        return source[charIndex];
    };

    [[nodiscard]] static auto PeekCurrentChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 0); };
    [[nodiscard]] static auto PeekNextChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 1); };

    static auto LexKeyword(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        currentIndex++;

        if (PeekCurrentChar(source, currentIndex) == QChar('-'))
            currentIndex++;

        while (PeekCurrentChar(source, currentIndex).isLetter())
            currentIndex++;

        const auto length = currentIndex - startIndex;
        const auto keyword = source.sliced(startIndex, length);
        if (keyword == QString("#true"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_True, startIndex, currentIndex);
            return;
        }
        else if (keyword == QString("#false"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_False, startIndex, currentIndex);
            return;
        }
        else if (keyword == QString("#inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_Infinity, startIndex, currentIndex);
            return;
        }
        else if (keyword == QString("#-inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NegativeInfinity, startIndex, currentIndex);
            return;
        }
        else if (keyword == QString("#nan"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NaN, startIndex, currentIndex);
            return;
        }

        tokenBuffer.addToken(TokenKind::Unknown, startIndex, currentIndex);
    };
}

namespace KDL 
{
    TokenBuffer Lex(const QString& source) noexcept
    {
        TokenBuffer buffer{ source };
        i32 currentIndex = 0;

        while (true)
        {
            auto current = PeekCurrentChar(source, currentIndex);
            switch (current.unicode())
            {
                case '=':
                {
                    buffer.addToken(TokenKind::Equal, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case '(':
                {
                    buffer.addToken(TokenKind::OpenParenthesis, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case ')':
                {
                    buffer.addToken(TokenKind::CloseParenthesis, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case '{':
                {
                    buffer.addToken(TokenKind::OpenBracket, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case '}':
                {
                    buffer.addToken(TokenKind::CloseBracket, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case ';':
                {
                    buffer.addToken(TokenKind::Terminator, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case '#':
                {
                    LexKeyword(buffer, source, currentIndex);
                    break;
                }
                case '\0':
                {
                    buffer.addToken(TokenKind::EndOfFile, currentIndex, currentIndex);
                    currentIndex++;
                    return buffer;
                }
                default:
                    buffer.addToken(TokenKind::Unknown, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
            }
        }

        return buffer;
    }
}
