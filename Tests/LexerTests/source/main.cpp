#include <AalTest.h>
#include "LexerTests.h"

int main()
{
    AalTest::TestRunner runner{};
    auto testSuites = LexerTestsSuite();

    runner.run(testSuites);

    return 0;
}
