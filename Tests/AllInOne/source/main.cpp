#include <AalTest.h>
#include "../../LexerTests/source/LexerTests.h"
//#include "../../ParserTests/source/ParserTests.h"

int main()
{
    TestRunner runner{};
    QList<TestSuite> testSuites{};
    testSuites << LexerTestsSuite();
    //testSuites << ParserTestsSuite();

    runner.run(testSuites);

    return 0;
}
