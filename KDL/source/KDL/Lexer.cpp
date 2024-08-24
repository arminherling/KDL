#include <KDL/Lexer.h>
#include <functional>

namespace
{
    using namespace KDL;

    struct BoolSizePair
    {
        bool Bool;
        uint Size;
    };

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

    [[nodiscard]] static auto IsHash(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return current == QChar(u'#');
    }

    [[nodiscard]] static auto IsQuote(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return current == QChar(u'\"');
    }

    [[nodiscard]] static auto IsSign(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return (current == QChar(u'-') || current == QChar(u'+'));
    }

    [[nodiscard]] static auto IsDot(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return (current == QChar(u'.'));
    }

    [[nodiscard]] static auto IsEOF(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return (current == QChar(u'\0'));
    }

    [[nodiscard]] static auto IsDigit(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        return current.isNumber();
    }

    [[nodiscard]] static BoolSizePair IsEscapeSequence(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        const auto next = PeekCurrentChar(source, currentIndex + 1);

        if (current != QChar(u'\\'))
            return { .Bool = false, .Size = 1 };

        switch (next.unicode())
        {
            case u'n': // Line Feed
            case u'r': // Carriage Return
            case u't': // Character Tabulation (Tab)
            case u'\\': // Reverse Solidus (Backslash)
            case u'\"': // Quotation Mark (Double Quote)
            case u'b': // Backspace
            case u'f': // Form Feed
            case u's': // Space
            {
                return { .Bool = true, .Size = 2 };
            }
            case u'u':  // Unicode Escape
            {
                auto escapeIndex = currentIndex + 2;
                if (PeekCurrentChar(source, escapeIndex) != QChar('{'))
                    break;
                escapeIndex++;

                auto hexCharCount = 0;
                while (hexCharCount != 6 && IsHexadecimal(PeekCurrentChar(source, escapeIndex)))
                {
                    escapeIndex++;
                    hexCharCount++;
                }

                if (PeekCurrentChar(source, escapeIndex) != QChar('}'))
                    break;
                escapeIndex++;

                uint length = (escapeIndex - currentIndex);
                return { .Bool = true, .Size = length };
            }
            default:
                if (next.isSpace())  // Whitespace Escape
                {
                    auto escapeIndex = currentIndex + 2;
                    while (PeekCurrentChar(source, escapeIndex).isSpace())
                    {
                        escapeIndex++;
                    }

                    uint length = (escapeIndex - currentIndex);
                    return { .Bool = true, .Size = length };
                }
                break;  // TODO \ followed by any other character isnt allowed, handle error
        }

        return { .Bool = false, .Size = 1 };
    }

    [[nodiscard]] static BoolSizePair IsNewline(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        const auto next = PeekCurrentChar(source, currentIndex + 1);

        if (current == QChar(u'\r') && next == QChar(u'\n'))    // Carriage Return Line Feed
            return { .Bool = true, .Size = 2 };

        if (current == QChar(u'\r')                             // Carriage Return
            || current == QChar(u'\n')                          // Line Feed
            || current == QChar(u'\f')                          // Form Feed
            || current == QChar(u'\u0085')                      // Next Line
            || current == QChar(u'\u2028')                      // Line Separator
            || current == QChar(u'\u2029'))                     // Paragraph Separator
            return { .Bool = true, .Size = 1 };

        return { .Bool = false, .Size = 0 };
    }

    [[nodiscard]] static BoolSizePair IsEqual(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);
        const auto next = PeekCurrentChar(source, currentIndex + 1);

        if (current == QChar(u'=')
            || current == QChar(u'\ufe66')                      // Small Equal Sign
            || current == QChar(u'\uff1d'))                     // Fullwidth Equal Sign
            return { .Bool = true, .Size = 1 };

        if (current == QChar(0xD83D) && next == QChar(0xDFF0))  // Heavy Equal Sign
            return { .Bool = true, .Size = 2 };

        return { .Bool = false, .Size = 0 };
    }

    [[nodiscard]] static auto IsDisallowedIdentifierChar(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);

        return current == QChar(u'\\')
            || current == QChar(u'/')
            || current == QChar(u'(')
            || current == QChar(u')')
            || current == QChar(u'{')
            || current == QChar(u'}')
            || current == QChar(u';')
            || current == QChar(u'[')
            || current == QChar(u']')
            || current == QChar(u'\"')
            || current == QChar(u'#');
    }

    [[nodiscard]] static auto IsDisallowedLiteralCodePoints(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);

        return (current >= QChar(u'\u0000') && current <= QChar(u'\u0008'))
            || (current >= QChar(u'\u000e') && current <= QChar(u'\u001f')) // various control characters
            || current == QChar(u'\u007f')                                  // delete control character
            || (current >= QChar(u'\u200f') && current <= QChar(u'\u200f')) // unicode "direction control" characters
            || (current >= QChar(u'\u202a') && current <= QChar(u'\u202e')) // unicode "direction control" characters
            || (current >= QChar(u'\u2066') && current <= QChar(u'\u2069')) // unicode "direction control" characters
            || current == QChar(u'\ufeff');                                 // zero-width non-breaking space / byte order mark 
    }

    [[nodiscard]] static auto IsIdentifierChar(const QString& source, const i32& currentIndex) noexcept
    {
        const auto current = PeekCurrentChar(source, currentIndex);

        return !current.isSpace()
            && !IsNewline(source, currentIndex).Bool
            && !IsDisallowedIdentifierChar(source, currentIndex)
            && !IsDisallowedLiteralCodePoints(source, currentIndex)
            && !IsEqual(source, currentIndex).Bool;
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

        if (PeekCurrentChar(source, currentIndex) == QChar(u'-'))
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
            return false;
        }

        return true;
    };

    static auto TryLexNumber(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;

        auto currentChar = PeekCurrentChar(source, currentIndex);
        auto nextChar = PeekNextChar(source, currentIndex);
        if (IsSign(source, currentIndex))
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

    static auto TryLexQuotedString(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (!IsQuote(source, currentIndex))
            return false;

        currentIndex++;

        while (true)
        {
            if (auto result = IsEscapeSequence(source, currentIndex); result.Bool)
            {
                currentIndex += result.Size;
                continue;
            }

            if (!IsQuote(source, currentIndex) && !IsEOF(source, currentIndex))
            {
                currentIndex++;
                continue;
            }

            if (!IsEOF(source, currentIndex))
                currentIndex++;

            break;
        }

        tokenBuffer.addToken(TokenKind::Identifier_QuotedString, startIndex, currentIndex);
        return true;
    }

    static auto TryLexRawString(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        auto startHashCount = 0;
        while (IsHash(source, currentIndex))
        {
            currentIndex++;
            startHashCount++;
        }

        if (!IsQuote(source, currentIndex))
        {
            currentIndex = startIndex;
            return false;
        }
        currentIndex++;

        while (true)
        {
            if (!IsQuote(source, currentIndex) && !IsEOF(source, currentIndex))
            {
                currentIndex++;
                continue;
            }

            if (IsQuote(source, currentIndex))
            {
                currentIndex++;
                auto endHasCount = 0;

                while (IsHash(source, currentIndex))
                {
                    currentIndex++;
                    endHasCount++;
                }

                if (startHashCount == endHasCount)
                    break;

                continue;
            }
            else if (!IsEOF(source, currentIndex))
                currentIndex++;

            break;
        }

        tokenBuffer.addToken(TokenKind::Identifier_RawString, startIndex, currentIndex);
        return true;
    }
    static auto TryLexDottedIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (IsSign(source, currentIndex))
            currentIndex++;

        if (!IsDot(source, currentIndex))
            return false;

        currentIndex++;

        if (IsIdentifierChar(source, currentIndex) && !IsDigit(source, currentIndex))
            currentIndex++;

        while (IsIdentifierChar(source, currentIndex))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexSignedIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (!IsSign(source, currentIndex))
            return false;

        currentIndex++;

        if (IsIdentifierChar(source, currentIndex) && !IsDigit(source, currentIndex) && !IsDot(source, currentIndex))
            currentIndex++;

        while (IsIdentifierChar(source, currentIndex))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexUnambiguousIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        if (IsIdentifierChar(source, currentIndex) && !IsDigit(source, currentIndex) && !IsSign(source, currentIndex) && !IsDot(source, currentIndex))
            currentIndex++;
        else
            return false;

        while (IsIdentifierChar(source, currentIndex))
        {
            currentIndex++;
        }

        tokenBuffer.addToken(TokenKind::Identifier, startIndex, currentIndex);
        return true;
    }

    static auto TryLexIdentifier(TokenBuffer& tokenBuffer, const QString& source, i32& currentIndex) noexcept
    {
        if (IsHash(source, currentIndex))
        {
            if (!IsHash(source, currentIndex + 1) && !IsQuote(source, currentIndex + 1))
            {
                return LexKeyword(tokenBuffer, source, currentIndex);
            }
            return TryLexRawString(tokenBuffer, source, currentIndex);
        }
        else if (IsQuote(source, currentIndex))
        {
            return TryLexQuotedString(tokenBuffer, source, currentIndex);
        }
        else if ((IsSign(source, currentIndex) && IsDot(source, currentIndex))
            || IsDot(source, currentIndex))
        {
            return TryLexDottedIdentifier(tokenBuffer, source, currentIndex);
        }
        else if (IsSign(source, currentIndex))
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
                case u';':
                {
                    buffer.addToken(TokenKind::Terminator, currentIndex, currentIndex + 1);
                    currentIndex++;
                    break;
                }
                case u'\0':
                {
                    buffer.addToken(TokenKind::EndOfFile, currentIndex, currentIndex);
                    return buffer;
                }
                case u'/':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'-')) // Slash dash
                    {
                        buffer.addToken(TokenKind::SlashDash, currentIndex, currentIndex + 2);
                        currentIndex += 2;
                        break;
                    }
                    else if (PeekNextChar(source, currentIndex) == QChar(u'/')) // Line comments
                    {
                        auto startIndex = currentIndex;
                        currentIndex += 2;
                        while (true)
                        {
                            if (const auto result = IsNewline(source, currentIndex); !result.Bool)
                            {
                                if (IsEOF(source, currentIndex))
                                    break;

                                currentIndex++;
                            }
                            else
                            {
                                currentIndex += result.Size;
                                break;
                            }
                        }
                        break;
                    }
                    else if (PeekNextChar(source, currentIndex) == QChar(u'*')) // Block comments
                    {
                        auto startIndex = currentIndex;
                        currentIndex += 2;
                        auto nestingLevel = 1;
                        while (true)
                        {
                            if (PeekCurrentChar(source, currentIndex) == QChar(u'*') && PeekNextChar(source, currentIndex) == QChar(u'/'))
                            {
                                currentIndex += 2;
                                nestingLevel--;
                            }
                            else if (PeekCurrentChar(source, currentIndex) == QChar(u'/') && PeekNextChar(source, currentIndex) == QChar(u'*'))
                            {
                                currentIndex += 2;
                                nestingLevel++;
                            }
                            else
                            {
                                currentIndex++;
                            }

                            if (nestingLevel == 0)
                                break;
                        }
                        break;
                    }
                    [[fallthrough]];
                }
                default:
                {
                    if (auto result = IsNewline(source, currentIndex); result.Bool)
                    {
                        buffer.addToken(TokenKind::Newline, currentIndex, currentIndex + result.Size);
                        currentIndex += result.Size;
                        break;
                    }
                    else if (current.isSpace())
                    {
                        currentIndex++;
                        break;
                    }
                    else if (auto result = IsEqual(source, currentIndex); result.Bool)
                    {
                        buffer.addToken(TokenKind::Equal, currentIndex, currentIndex + result.Size);
                        currentIndex += result.Size;
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
