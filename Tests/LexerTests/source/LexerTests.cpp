#include <AalTest.h>

#include <KDL/Lexer.h>
#include <KDL/TokenKind.h>
#include <KDL/TokenBuffer.h>

#include <QDirIterator>
#include <QFile>

#include <iostream>

namespace
{
    using namespace KDL;

    void NoUnknownTokens(const QString& fileName, const QString& filePath)
    {
        auto file = QFile(filePath);
        const auto isOpen = file.open(QIODevice::ReadOnly | QIODevice::Text);
        AalTest::IsTrue(isOpen);
        QTextStream reader(&file);
        const auto input = reader.readAll();

        const auto tokens = Lex(input);
        for (size_t i = 0; i < tokens.size(); i++)
        {
            const auto token = tokens[i];
            if (token.kind == TokenKind::Unknown)
                AalTest::Fail();
        }
    }

    QList<std::tuple<QString, QString>> NoUnknownTokens_Data()
    {
        auto inputDir = QDir(QString("../../Tests/Data/Input"));
        auto inputAbsolutePath = inputDir.absolutePath();
        auto expectedDir = QDir(QString("../../Tests/Data/Expected"));
        auto expectedAbsolutePath = expectedDir.absolutePath();
        auto exampleDir = QDir(QString("../../Tests/Data/Examples"));
        auto exampleAbsolutePath = exampleDir.absolutePath();

        QList<std::tuple<QString, QString>> data{};

        QDirIterator inputIt(inputAbsolutePath, QStringList() << QString("*.kdl"), QDir::Filter::Files);
        while (inputIt.hasNext())
        {
            auto file = inputIt.nextFileInfo();
            auto fileName = file.fileName();
            auto filePath = file.absoluteFilePath();

            data.append(std::make_tuple(fileName, filePath));
        }

        QDirIterator expectedIt(expectedAbsolutePath, QStringList() << QString("*.kdl"), QDir::Filter::Files);
        while (expectedIt.hasNext())
        {
            auto file = expectedIt.nextFileInfo();
            auto fileName = file.fileName();
            auto filePath = file.absoluteFilePath();

            data.append(std::make_tuple(fileName, filePath));
        }

        QDirIterator exampleIt(exampleAbsolutePath, QStringList() << QString("*.kdl"), QDir::Filter::Files);
        while (exampleIt.hasNext())
        {
            auto file = exampleIt.nextFileInfo();
            auto fileName = file.fileName();
            auto filePath = file.absoluteFilePath();

            data.append(std::make_tuple(fileName, filePath));
        }
        return data;
    }

    void QtHandlesWhitespace(const QString& testName, const QString& source)
    {
        AalTest::IsTrue(source[0].isSpace());
    }

    QList<std::tuple<QString, QString>> Whitespace_Data()
    {
        return {
            { QString("Character Tabulation"), QString("\u0009") },
            { QString("Line Tabulation"), QString("\u000b") },
            { QString("Space"), QString("\u0020") },
            { QString("No-Break Space"), QString("\u00a0") },
            { QString("Ogham Space Mark"), QString("\u1680") },
            { QString("En Quad"), QString("\u2000") },
            { QString("Em Quad"), QString("\u2001") },
            { QString("En Space"), QString("\u2002") },
            { QString("Em Space"), QString("\u2003") },
            { QString("Three-Per-Em Space"), QString("\u2004") },
            { QString("Four-Per-Em Space"), QString("\u2005") },
            { QString("Six-Per-Em Space"), QString("\u2006") },
            { QString("Figure Space"), QString("\u2007") },
            { QString("Punctuation Space"), QString("\u2008") },
            { QString("Thin Space"), QString("\u2009") },
            { QString("Hair Space"), QString("\u200a") },
            { QString("Narrow No-Break Space"), QString("\u202f") },
            { QString("Medium Mathematical Space"), QString("\u205f") },
            { QString("Ideographic Space"), QString("\u3000") }
        };
    }

    void Compare(const QString& testName, const QString& source, TokenKind expectedKind, i32 tokenCount)
    {
        const auto tokens = Lex(source);
        const auto token = tokens[0];

        AalTest::AreEqual(expectedKind, token.kind);
        AalTest::AreEqual(tokenCount, tokens.size());
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> SingleCharacter_Data()
    {
        return {
            { QString("OpenParenthesis"), QString("("), TokenKind::OpenParenthesis, 2 },
            { QString("CloseParenthesis"), QString(")"), TokenKind::CloseParenthesis, 2 },
            { QString("OpenBracket"), QString("{"), TokenKind::OpenBracket, 2 },
            { QString("CloseBracket"), QString("}"), TokenKind::CloseBracket, 2 },
            { QString("Semicolon"), QString(";"), TokenKind::Terminator, 2 },

            { QString("EOF"), QString(""), TokenKind::EndOfFile, 1 },
            { QString("EOF \\0"), QString("\0"), TokenKind::EndOfFile, 1 }
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Equal_Data()
    {
        return {
            { QString("Equal"), QString("="), TokenKind::Equal, 2 },
            { QString("Small Equal Sign"), QString("\ufe66"), TokenKind::Equal, 2 },
            { QString("Fullwidth Equal Sign"), QString("\uff1d"), TokenKind::Equal, 2 },
            { QString("Heavy Equal Sign"), QString("\U0001f7f0"), TokenKind::Equal, 2 }
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Newline_Data()
    {
        return {
            { QString("Carriage Return"), QString("\r"), TokenKind::Newline, 2 },
            { QString("Line Feed"), QString("\n"), TokenKind::Newline, 2 },
            { QString("Carriage Return and Line Feed"), QString("\r\n"), TokenKind::Newline, 2 },
            { QString("Next Line"), QString("\u0085"), TokenKind::Newline, 2 },
            { QString("Form Feed"), QString("\f"), TokenKind::Newline, 2 },
            { QString("Line Separator"), QString(QChar::SpecialCharacter::LineSeparator), TokenKind::Newline, 2 },
            { QString("Paragraph Separator"), QString(QChar::SpecialCharacter::ParagraphSeparator), TokenKind::Newline, 2 }
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> SlashDash_Data()
    {
        return {
            { QString("SlashDash"), QString("/-"), TokenKind::SlashDash, 2 },
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Sign_Data()
    {
        return {
            { QString("-"), QString("-"), TokenKind::Identifier, 2 },
            { QString("+"), QString("+"), TokenKind::Identifier, 2 },
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Keywords_Data()
    {
        return {
            { QString("True"), QString("#true"), TokenKind::Keyword_True, 2 },
            { QString("False"), QString("#false"),  TokenKind::Keyword_False, 2 },
            { QString("NaN"), QString("#nan"), TokenKind::Keyword_NaN, 2 },
            { QString("Inf"), QString("#inf"),  TokenKind::Keyword_Infinity, 2 },
            { QString("-Inf"), QString("#-inf"), TokenKind::Keyword_NegativeInfinity, 2 },
            { QString("Null"), QString("#null"), TokenKind::Keyword_Null, 2 },
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Number_Data()
    {
        return {
            { QString(), QString("0b0"), TokenKind::Number_Binary, 2 },
            { QString(), QString("0b1"), TokenKind::Number_Binary, 2 },
            { QString(), QString("0b101"), TokenKind::Number_Binary, 2 },
            { QString(), QString("0b1_0_1"), TokenKind::Number_Binary, 2 },

            { QString(), QString("0o0"), TokenKind::Number_Octal, 2 },
            { QString(), QString("0o7"), TokenKind::Number_Octal, 2 },
            { QString(), QString("0o71"), TokenKind::Number_Octal, 2 },
            { QString(), QString("0o1_0_7"), TokenKind::Number_Octal, 2 },

            { QString(), QString("0x0"), TokenKind::Number_Hexadecimal, 2 },
            { QString(), QString("0xA"), TokenKind::Number_Hexadecimal, 2 },
            { QString(), QString("0x01F"), TokenKind::Number_Hexadecimal, 2 },
            { QString(), QString("0x1_0_F"), TokenKind::Number_Hexadecimal, 2 },
            { QString(), QString("0xABCDEF0123456789abcdef"), TokenKind::Number_Hexadecimal, 2 },

            { QString(), QString("1"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("10"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1_0"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1.0"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1_0.0_0"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1_0_0.0_0_0"), TokenKind::Number_Decimal, 2 },

            { QString(), QString("1.23e5"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1.23E+1000"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1.23E-1000"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1.23E-10E-00"), TokenKind::Number_Decimal, 2 },
            { QString(), QString("1.0E10e10"), TokenKind::Number_Decimal, 2 },
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> Identifier_Data()
    {
        return {
            { QString(), QString("node"), TokenKind::Identifier, 2},
            { QString(), QString("+node"), TokenKind::Identifier, 2 },
            { QString(), QString("-node"), TokenKind::Identifier, 2},
            { QString(), QString("+.node"), TokenKind::Identifier, 2 },
            { QString(), QString("-.node"), TokenKind::Identifier, 2},
            { QString(), QString(".node"), TokenKind::Identifier, 2},
            { QString(), QString("."), TokenKind::Identifier, 2},
            { QString(), QString("+."), TokenKind::Identifier, 2},
            { QString(), QString("$"), TokenKind::Identifier, 2 },
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> QuotedString_Data()
    {
        return {
            { QString(), QString("\"node\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Line Feed"), QString("\"\\n\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Carriage Return"), QString("\"\\r\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Character Tabulation (Tab)"), QString("\"\\t\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Reverse Solidus (Backslash)"), QString("\"\\\\\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Quotation Mark (Double Quote)"), QString("\"\\\"\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Backspace"), QString("\"\\b\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Form Feed"), QString("\"\\f\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Space"), QString("\"\\s\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Unicode Escape"), QString("\"\\u{000000}\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Unicode Escape"), QString("\"\\u{FFFFFF}\""), TokenKind::Identifier_QuotedString, 2},
            { QString("Whitespace Escape"), QString("\"\\        \""), TokenKind::Identifier_QuotedString, 2},
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> RawString_Data()
    {
        return {
            { QString(), QString("#\"node\"#"), TokenKind::Identifier_RawString, 2},
            { QString(), QString("##\"hello\\n\\r\\asd\"#world\"##"), TokenKind::Identifier_RawString, 2},
            { QString(), QString("#\"hello\\n\\r\\asd\"##world\"#"), TokenKind::Identifier_RawString, 2},
        };
    }

    QList<std::tuple<QString, QString, TokenKind, i32>> LineComment_Data()
    {
        return {
            { QString(), QString("//node"), TokenKind::EndOfFile, 1},
        };
    }
}

AalTest::TestSuite LexerTestsSuite()
{
    AalTest::TestSuite suite{};
    suite.add(QString("NoUnknownTokens"), NoUnknownTokens, NoUnknownTokens_Data);
    suite.add(QString("Whitespace"), QtHandlesWhitespace, Whitespace_Data);
    suite.add(QString("SingleCharacter"), Compare, SingleCharacter_Data);
    suite.add(QString("Equal"), Compare, Equal_Data);
    suite.add(QString("Newline"), Compare, Newline_Data);
    suite.add(QString("SlashDash"), Compare, SlashDash_Data);
    suite.add(QString("Sign"), Compare, Sign_Data);
    suite.add(QString("Keywords"), Compare, Keywords_Data);
    suite.add(QString("Number"), Compare, Number_Data);
    suite.add(QString("Identifier"), Compare, Identifier_Data);
    suite.add(QString("QuotedString"), Compare, QuotedString_Data);
    suite.add(QString("RawString"), Compare, RawString_Data);
    suite.add(QString("LineComment"), Compare, LineComment_Data);

    return suite;
}
