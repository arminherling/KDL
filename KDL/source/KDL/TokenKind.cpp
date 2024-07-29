#include <KDL/TokenKind.h>
#include <Defines.h>

namespace KDL 
{
    QString Stringify(TokenKind kind)
    {
        switch (kind)
        {
            case TokenKind::Unknown:
                return QString("Unknown");
            case TokenKind::Error:
                return QString("Error");
            case TokenKind::Equal:
                return QString("Equal");
            case TokenKind::OpenParenthesis:
                return QString("OpenParenthesis");
            case TokenKind::CloseParenthesis:
                return QString("CloseParenthesis");
            case TokenKind::OpenBracket:
                return QString("OpenBracket");
            case TokenKind::CloseBracket:
                return QString("CloseBracket");
            case TokenKind::Newline:
                return QString("Newline");
            case TokenKind::Terminator:
                return QString("Terminator");
            case TokenKind::Keyword_True:
                return QString("Keyword_True");
            case TokenKind::Keyword_False:
                return QString("Keyword_False");
            case TokenKind::Keyword_NaN:
                return QString("Keyword_NaN");
            case TokenKind::Keyword_Infinity:
                return QString("Keyword_Infinity");
            case TokenKind::Keyword_NegativeInfinity:
                return QString("Keyword_NegativeInfinity");
            case TokenKind::EndOfFile:
                return QString("EndOfFile");
            default:
                TODO("String for TokenKind value was not defined yet");
        }
    }
}
