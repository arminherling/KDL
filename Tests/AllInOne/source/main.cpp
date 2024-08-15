#include <AalTest.h>
#include "../../LexerTests/source/LexerTests.h"
#include "../../ParserTests/source/ParserTests.h"

int main()
{
    AalTest::TestRunner runner{};
    QList<AalTest::TestSuite> testSuites{};
    testSuites << LexerTestsSuite();
    testSuites << ParserTestsSuite();

    runner.run(testSuites);

    return 0;
}
