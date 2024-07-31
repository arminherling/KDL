#include <KDL/Lexer.h>
#include <functional>

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

    [[nodiscard]] static auto IsBinary(const QChar& nextChar) noexcept
    {
        return (nextChar == QChar('0') || nextChar == QChar('1'));
    }

    [[nodiscard]] static auto IsOctal(const QChar& nextChar) noexcept
    {
        return (nextChar >= QChar('0') && nextChar <= QChar('7'));
    }

    [[nodiscard]] static auto IsHexadecimal(const QChar& nextChar) noexcept
    {
        return (nextChar >= QChar('0') && nextChar <= QChar('9'))
            || (nextChar >= QChar('a') && nextChar <= QChar('f'))
            || (nextChar >= QChar('A') && nextChar <= QChar('F'));
    }

    static auto NumberType(const QChar c) noexcept
    {
        if (c == 'b')
            return TokenKind::Number_Binary;
        if (c == 'o')
            return TokenKind::Number_Octal;
        if (c == 'x')
            return TokenKind::Number_Hexadecimal;
        return TokenKind::Number_Decimal;
    }

    static std::function<bool(const QChar)> NumberPredicate(TokenKind type)
    {
        switch (type)
        {
            case TokenKind::Number_Binary:
                return [](const QChar c) {return IsBinary(c); };
            case TokenKind::Number_Octal:
                return [](const QChar c) {return IsOctal(c); };
            case TokenKind::Number_Hexadecimal:
                return [](const QChar c) {return IsHexadecimal(c); };
            case TokenKind::Number_Decimal:
                return [](const QChar c) {return c.isNumber(); };
            default:
                __debugbreak();
        }
    }

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
        }
        else if (keyword == QString("#false"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_False, startIndex, currentIndex);
        }
        else if (keyword == QString("#inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_Infinity, startIndex, currentIndex);
        }
        else if (keyword == QString("#-inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NegativeInfinity, startIndex, currentIndex);
        }
        else if (keyword == QString("#nan"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NaN, startIndex, currentIndex);
        }
        else
        {
            tokenBuffer.addToken(TokenKind::Unknown, startIndex, currentIndex);
        }
    };

    static auto TryLexNumber(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;

        auto currentChar = PeekCurrentChar(source, currentIndex);
        auto nextChar = PeekNextChar(source, currentIndex);
        if (currentChar == '+' || currentChar == '-')
        {
            if (nextChar.isNumber())
            {
                currentIndex += 2;
            }
            else
            {
                return false;
            }
        }

        currentChar = PeekCurrentChar(source, currentIndex);
        if (!currentChar.isNumber())
            return false;

        nextChar = PeekNextChar(source, currentIndex);
        const auto numberType = NumberType(nextChar);
        const auto predicate = NumberPredicate(numberType);

        auto matchNumbersAndUnderscores = [&]()
            {
                currentChar = PeekCurrentChar(source, currentIndex);
                while (predicate(currentChar))
                {
                    currentIndex++;
                    currentChar = PeekCurrentChar(source, currentIndex);

                    nextChar = PeekNextChar(source, currentIndex);
                    while (currentChar == '_' && predicate(nextChar))
                    {
                        currentIndex += 2;
                        currentChar = PeekCurrentChar(source, currentIndex);
                        nextChar = PeekNextChar(source, currentIndex);
                    }
                }
            };

        if (numberType != TokenKind::Number_Decimal)
        {
            currentIndex += 2;

            matchNumbersAndUnderscores();

            tokenBuffer.addToken(numberType, startIndex, currentIndex);
            return true;
        }
        else  // Decimal
        {
            matchNumbersAndUnderscores();

            if (currentChar == '.' && PeekNextChar(source, currentIndex).isNumber())
            {
                currentIndex++;

                matchNumbersAndUnderscores();
            }

            nextChar = PeekNextChar(source, currentIndex);
            while (currentChar.toLower() == 'e' && (nextChar == '+' || nextChar == '-' || nextChar.isNumber()))
            {
                if (nextChar == '+' || nextChar == '-')
                {
                    currentIndex += 2;
                }
                else
                {
                    currentIndex++;
                }

                matchNumbersAndUnderscores();
            }

            tokenBuffer.addToken(TokenKind::Number_Decimal, startIndex, currentIndex);
            return true;
        }

        return false;
    }

    static auto TryLexIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        if (!PeekCurrentChar(source, currentIndex).isLetter())
            return false;

        const auto startIndex = currentIndex;
        while (PeekCurrentChar(source, currentIndex).isLetter())
        {
            currentIndex++;
        }
        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);

        return true;
    }
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
                case u'\r':     // Carriage Return
                {
                    const auto startIndex = currentIndex;
                    currentIndex++;
                    if (PeekCurrentChar(source, currentIndex) == QChar('\n'))
                        currentIndex++;

                    buffer.addToken(TokenKind::Newline, startIndex, currentIndex);
                    break;
                }
                case u'\n':     // Line Feed
                case u'\f':     // Form Feed
                case u'\u0085': // Next Line
                case u'\u2028': // Line Separator
                case u'\u2029': // Paragraph Separator
                {
                    buffer.addToken(TokenKind::Newline, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'=':
                case u'\ufe66': // Small Equal Sign
                case u'\uff1d': // Fullwidth Equal Sign
                {
                    buffer.addToken(TokenKind::Equal, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case 0xD83D: // Heavy Equal Sign
                {
                    if (PeekNextChar(source, currentIndex).unicode() == 0xDFF0)
                    {
                        buffer.addToken(TokenKind::Equal, currentIndex, currentIndex + 2);
                        currentIndex += 2;
                        break;
                    }
                    goto default_case;
                }
                case u'(':
                {
                    buffer.addToken(TokenKind::OpenParenthesis, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u')':
                {
                    buffer.addToken(TokenKind::CloseParenthesis, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'{':
                {
                    buffer.addToken(TokenKind::OpenBracket, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'}':
                {
                    buffer.addToken(TokenKind::CloseBracket, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'/':
                {
                    if (PeekNextChar(source, currentIndex) == QChar('-'))
                    {
                        buffer.addToken(TokenKind::SlashDash, currentIndex, currentIndex + 2);
                        currentIndex += 2;
                        break;
                    }

                    // TODO handle comments
                    goto default_case;
                }
                case u';':
                {
                    buffer.addToken(TokenKind::Terminator, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'#':
                {
                    LexKeyword(buffer, source, currentIndex);
                    break;
                }
                case u'\0':
                {
                    buffer.addToken(TokenKind::EndOfFile, currentIndex, currentIndex);
                    return buffer;
                }
                default:
                {
                default_case:
                    if (current.isSpace())
                    {
                        currentIndex++;
                        break;
                    }
                    else if (TryLexNumber(buffer, source, currentIndex))
                    {
                        break;
                    }
                    else if (TryLexIdentifier(buffer, source, currentIndex))
                    {
                        break;
                    }

                    buffer.addToken(TokenKind::Unknown, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
            }
        }

        return buffer;
    }
}
