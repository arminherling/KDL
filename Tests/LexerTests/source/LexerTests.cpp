#include <AalTest.h>

#include <KDL/Lexer.h>
#include <KDL/TokenKind.h>
#include <KDL/TokenBuffer.h>
#include <iostream>

namespace
{
    using namespace KDL;

    void SingleCharacter(const QString& testName, const QString& source, TokenKind expectedKind)
    {
        Stringify(TokenKind::Equal);

        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto tokens = Lex(source);
        const auto token = tokens[0];

        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      Lex(): " << Stringify(endTime - startTime).toStdString() << '\n';

        AalTest::AreEqual(expectedKind, token.kind);
    }

    QList<std::tuple<QString, QString, TokenKind>> SingleCharacter_Data()
    {
        return {
            std::make_tuple(QString("Equal"), QString("="), TokenKind::Equal),
            std::make_tuple(QString("OpenParenthesis"), QString("("), TokenKind::OpenParenthesis),
            std::make_tuple(QString("CloseParenthesis"), QString(")"), TokenKind::CloseParenthesis),
            std::make_tuple(QString("OpenBracket"), QString("{"), TokenKind::OpenBracket),
            std::make_tuple(QString("CloseBracket"), QString("}"), TokenKind::CloseBracket),
            std::make_tuple(QString("Semicolon"), QString(";"), TokenKind::Terminator),

            std::make_tuple(QString("Unknown"), QString("$"), TokenKind::Unknown),
            std::make_tuple(QString("EOF"), QString(""), TokenKind::EndOfFile),
            std::make_tuple(QString("EOF \\0"), QString("\0"), TokenKind::EndOfFile)
        };
    }

    void Keywords(const QString& source, TokenKind expectedKind)
    {
        Stringify(TokenKind::Equal);

        const auto startTime = std::chrono::high_resolution_clock::now();

        const auto tokens = Lex(source);
        const auto token = tokens[0];

        const auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      Lex(): " << Stringify(endTime - startTime).toStdString() << '\n';

        AalTest::AreEqual(expectedKind, token.kind);
    }

    QList<std::tuple<QString, TokenKind>> Keywords_Data()
    {
        return {
            std::make_tuple(QString("#true"), TokenKind::Keyword_True),
            std::make_tuple(QString("#false"),  TokenKind::Keyword_False),
            std::make_tuple(QString("#nan"), TokenKind::Keyword_NaN),
            std::make_tuple(QString("#inf"),  TokenKind::Keyword_Infinity),
            std::make_tuple(QString("#-inf"), TokenKind::Keyword_NegativeInfinity)
        };
    }
}

TestSuite LexerTestsSuite()
{
    TestSuite suite{};
    suite.add(QString("SingleCharacter"), SingleCharacter, SingleCharacter_Data);
    suite.add(QString("Keywords"), Keywords, Keywords_Data);

    return suite;
}
