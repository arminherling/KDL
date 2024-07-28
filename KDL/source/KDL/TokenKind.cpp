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
            default:
                TODO("String for TokenKind value was not defined yet");
        }
    }
}
