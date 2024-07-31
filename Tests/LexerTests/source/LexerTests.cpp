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

    void Compare(const QString& testName, const QString& source, TokenKind expectedKind)
    {
        const auto tokens = Lex(source);
        const auto token = tokens[0];

        AalTest::AreEqual(expectedKind, token.kind);
    }

    QList<std::tuple<QString, QString, TokenKind>> SingleCharacter_Data()
    {
        return {
            { QString("OpenParenthesis"), QString("("), TokenKind::OpenParenthesis },
            { QString("CloseParenthesis"), QString(")"), TokenKind::CloseParenthesis },
            { QString("OpenBracket"), QString("{"), TokenKind::OpenBracket },
            { QString("CloseBracket"), QString("}"), TokenKind::CloseBracket },
            { QString("Semicolon"), QString(";"), TokenKind::Terminator },

            { QString("Unknown"), QString("$"), TokenKind::Unknown },
            { QString("EOF"), QString(""), TokenKind::EndOfFile },
            { QString("EOF \\0"), QString("\0"), TokenKind::EndOfFile }
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> Equal_Data()
    {
        return {
            { QString("Equal"), QString("="), TokenKind::Equal },
            { QString("Small Equal Sign"), QString("\ufe66"), TokenKind::Equal },
            { QString("Fullwidth Equal Sign"), QString("\uff1d"), TokenKind::Equal },
            { QString("Heavy Equal Sign"), QString("\U0001f7f0"), TokenKind::Equal }
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> Newline_Data()
    {
        return {
            { QString("Carriage Return"), QString("\r"), TokenKind::Newline },
            { QString("Line Feed"), QString("\n"), TokenKind::Newline },
            { QString("Carriage Return and Line Feed"), QString("\r\n"), TokenKind::Newline },
            { QString("Next Line"), QString("\u0085"), TokenKind::Newline },
            { QString("Form Feed"), QString("\f"), TokenKind::Newline },
            { QString("Line Separator"), QString(QChar::SpecialCharacter::LineSeparator), TokenKind::Newline },
            { QString("Paragraph Separator"), QString(QChar::SpecialCharacter::ParagraphSeparator), TokenKind::Newline }
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> SlashDash_Data()
    {
        return {
            { QString("SlashDash"), QString("/-"), TokenKind::SlashDash },
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> Keywords_Data()
    {
        return {
            { QString("True"), QString("#true"), TokenKind::Keyword_True },
            { QString("False"), QString("#false"),  TokenKind::Keyword_False },
            { QString("NaN"), QString("#nan"), TokenKind::Keyword_NaN },
            { QString("Inf"), QString("#inf"),  TokenKind::Keyword_Infinity },
            { QString("-Inf"), QString("#-inf"), TokenKind::Keyword_NegativeInfinity }
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> Identifier_Data()
    {
        return {
            { QString(), QString("node"), TokenKind::Identifier}
        };
    }

    QList<std::tuple<QString, QString, TokenKind>> Number_Data()
    {
        return {
            { QString(), QString("1"), TokenKind::Number_Decimal }
        };
    }
}

TestSuite LexerTestsSuite()
{
    TestSuite suite{};
    suite.add(QString("NoUnknownTokens"), NoUnknownTokens, NoUnknownTokens_Data);
    suite.add(QString("Whitespace"), QtHandlesWhitespace, Whitespace_Data);
    suite.add(QString("SingleCharacter"), Compare, SingleCharacter_Data);
    suite.add(QString("Equal"), Compare, Equal_Data);
    suite.add(QString("Newline"), Compare, Newline_Data);
    suite.add(QString("SlashDash"), Compare, SlashDash_Data);
    //suite.add(QString("Number"), Compare, Number_Data);
    suite.add(QString("Identifier"), Compare, Identifier_Data);
    suite.add(QString("Keywords"), Compare, Keywords_Data);

    return suite;
}
