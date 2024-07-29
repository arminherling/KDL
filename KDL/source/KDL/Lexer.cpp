#include <KDL/Lexer.h>

namespace 
{
    [[nodiscard]] static auto PeekChar(const QString& source, i32 currentIndex, i32 offset) noexcept
    {
        const auto charIndex = currentIndex + offset;
        if (charIndex >= source.length())
            return QChar('\0');

        return source[charIndex];
    };

    [[nodiscard]] static auto PeekCurrentChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 0); };
    [[nodiscard]] static auto PeekNextChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 1); };
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
