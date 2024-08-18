#include <KDL/Lexer.h>
#include <functional>

namespace
{
    using namespace KDL;

    [[nodiscard]] static auto PeekChar(const QString& source, i32 currentIndex, i32 offset) noexcept
    {
        const auto charIndex = currentIndex + offset;
        if (charIndex >= source.length())
            return QChar(u'\0');

        return source[charIndex];
    };

    [[nodiscard]] static auto PeekCurrentChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 0); };
    [[nodiscard]] static auto PeekNextChar(const QString& source, i32 currentIndex) noexcept { return PeekChar(source, currentIndex, 1); };

    [[nodiscard]] static auto IsBinary(const QChar& nextChar) noexcept
    {
        return (nextChar == QChar(u'0') || nextChar == QChar(u'1'));
    }

    [[nodiscard]] static auto IsOctal(const QChar& nextChar) noexcept
    {
        return (nextChar >= QChar(u'0') && nextChar <= QChar(u'7'));
    }

    [[nodiscard]] static auto IsHexadecimal(const QChar& nextChar) noexcept
    {
        return (nextChar >= QChar(u'0') && nextChar <= QChar(u'9'))
            || (nextChar >= QChar(u'a') && nextChar <= QChar(u'f'))
            || (nextChar >= QChar(u'A') && nextChar <= QChar(u'F'));
    }
    
    [[nodiscard]] static auto IsSign(const QChar& nextChar) noexcept
    {
        return (nextChar == QChar(u'-') || nextChar == QChar(u'+'));
    }

    [[nodiscard]] static auto IsDot(const QChar& nextChar) noexcept
    {
        return (nextChar == QChar(u'.'));
    }

    [[nodiscard]] static auto IsDigit(const QChar& nextChar) noexcept
    {
        return nextChar.isNumber();
    }

    [[nodiscard]] static auto IsIdentifierChar(const QChar& nextChar) noexcept
    {
        return nextChar.isLetter();
    }

    static auto NumberType(const QChar c, const QChar n) noexcept
    {
        if (c == QChar(u'0'))
        {
            if (n == QChar(u'b'))
                return TokenKind::Number_Binary;
            if (n == QChar(u'o'))
                return TokenKind::Number_Octal;
            if (n == QChar(u'x'))
                return TokenKind::Number_Hexadecimal;
        }
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
        else if (keyword == QString("#nan"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NaN, startIndex, currentIndex);
        }
        else if (keyword == QString("#inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_Infinity, startIndex, currentIndex);
        }
        else if (keyword == QString("#-inf"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_NegativeInfinity, startIndex, currentIndex);
        }
        else if (keyword == QString("#null"))
        {
            tokenBuffer.addToken(TokenKind::Keyword_Null, startIndex, currentIndex);
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
        if (IsSign(currentChar))
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
        const auto numberType = NumberType(currentChar, nextChar);
        const auto predicate = NumberPredicate(numberType);

        auto matchNumbersAndUnderscores = [&]()
            {
                currentChar = PeekCurrentChar(source, currentIndex);
                while (predicate(currentChar))
                {
                    currentIndex++;
                    currentChar = PeekCurrentChar(source, currentIndex);

                    nextChar = PeekNextChar(source, currentIndex);
                    while (currentChar == QChar(u'_') && predicate(nextChar))
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

            if (currentChar == QChar(u'.') && PeekNextChar(source, currentIndex).isNumber())
            {
                currentIndex++;

                matchNumbersAndUnderscores();
            }

            nextChar = PeekNextChar(source, currentIndex);
            while (currentChar.toLower() == QChar(u'e') && (nextChar == QChar(u'+') || nextChar == QChar(u'-') || nextChar.isNumber()))
            {
                if (nextChar == QChar(u'+') || nextChar == QChar(u'-'))
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

    static auto TryLexDottedIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (IsSign(PeekCurrentChar(source, currentIndex)))
            currentIndex++;

        if (!IsDot(PeekCurrentChar(source, currentIndex)))
            return false;

        currentIndex++;

        const auto firstChar = PeekCurrentChar(source, currentIndex);
        if (IsIdentifierChar(firstChar) && !IsDigit(firstChar))
            currentIndex++;

        while (IsIdentifierChar(PeekCurrentChar(source, currentIndex)))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexSignedIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (!IsSign(PeekCurrentChar(source, currentIndex)))
            return false;

        currentIndex++;

        const auto firstChar = PeekCurrentChar(source, currentIndex);
        if (IsIdentifierChar(firstChar) && !IsDigit(firstChar) && !IsDot(firstChar))
            currentIndex++;

        while (IsIdentifierChar(PeekCurrentChar(source, currentIndex)))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexUnambiguousIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        const auto firstChar = PeekCurrentChar(source, currentIndex);
        if (IsIdentifierChar(firstChar) && !IsDigit(firstChar) && !IsSign(firstChar) && !IsDot(firstChar))
            currentIndex++;
        else 
            return false;

        while (IsIdentifierChar(PeekCurrentChar(source, currentIndex)))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto firstChar = PeekCurrentChar(source, currentIndex);
        if ((IsSign(firstChar) && IsDot(PeekNextChar(source, currentIndex))) 
            || IsDot(firstChar))
        {
            return TryLexDottedIdentifier(tokenBuffer, source, currentIndex);
        }
        else if (IsSign(firstChar))
        {
            return TryLexSignedIdentifier(tokenBuffer, source, currentIndex);
        }
        else
        {
            return TryLexUnambiguousIdentifier(tokenBuffer, source, currentIndex);
        }
        
        return false;
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
