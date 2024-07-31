#include <KDL/TokenKind.h>
#include <Defines.h>

namespace KDL 
{
    QString Stringify(TokenKind kind)
    {
        switch (kind)
        {
            using enum TokenKind;

            case Unknown:
                return QString("Unknown");
            case Error:
                return QString("Error");
            case Newline:
                return QString("Newline");
            case Equal:
                return QString("Equal");
            case OpenParenthesis:
                return QString("OpenParenthesis");
            case CloseParenthesis:
                return QString("CloseParenthesis");
            case OpenBracket:
                return QString("OpenBracket");
            case CloseBracket:
                return QString("CloseBracket");
            case SlashDash:
                return QString("SlashDash");
            case Terminator:
                return QString("Terminator");
            case Number_Decimal:
                return QString("Number_Decimal");
            case Identifier:
                return QString("Identifier");
            case Keyword_True:
                return QString("Keyword_True");
            case Keyword_False:
                return QString("Keyword_False");
            case Keyword_NaN:
                return QString("Keyword_NaN");
            case Keyword_Infinity:
                return QString("Keyword_Infinity");
            case Keyword_NegativeInfinity:
                return QString("Keyword_NegativeInfinity");
            case EndOfFile:
                return QString("EndOfFile");
            default:
                TODO("String for TokenKind value was not defined yet");
        }
    }
}
