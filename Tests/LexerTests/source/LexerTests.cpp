#include <AalTest.h>

#include <KDL/Lexer.h>

namespace
{
    static void Fail()
    {
        AalTest::Fail();
    }
}

TestSuite LexerTestsSuite()
{
    TestSuite suite{};
    suite.add(QString("Fail"), Fail);

    return suite;
}
