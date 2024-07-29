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
            case TokenKind::Terminator:
                return QString("Terminator");
            case TokenKind::EndOfFile:
                return QString("EndOfFile");
            default:
                TODO("String for TokenKind value was not defined yet");
        }
    }
}
