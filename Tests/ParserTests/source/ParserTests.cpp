#include "ParserTests.h"
#include <AalTest.h>
#include <QDirIterator>
#include <QFile>

namespace
{
    void FileTests(const QString& fileName, const QString& inputFilePath, const QString& expectedFilePath)
    {
        AalTest::Fail();
    }

    QList<std::tuple<QString, QString, QString>> FileTests_Data()
    {
        auto inputDir = QDir(QString("../../Tests/Data/Input"));
        auto inputAbsolutePath = inputDir.absolutePath();
        auto expectedDir = QDir(QString("../../Tests/Data/Expected"));
        auto expectedAbsolutePath = expectedDir.absolutePath();

        QList<std::tuple<QString, QString, QString>> data{};

        QDirIterator it(inputAbsolutePath);
        while (it.hasNext())
        {
            auto file = QFileInfo(it.next());
            auto fileName = file.fileName();
            auto inputFilePath = inputDir.absoluteFilePath(fileName);
            auto expectedFilePath = expectedDir.absoluteFilePath(fileName);

            data.append(std::make_tuple(fileName, inputFilePath, expectedFilePath));
        }
        return data;
    }

    void Examples(const QString& fileName, const QString& inputFilePath)
    {
        AalTest::Fail();
    }

    QList<std::tuple<QString, QString>> Examples_Data()
    {
        auto inputDir = QDir(QString("../../Tests/Data/Examples"));
        auto inputAbsolutePath = inputDir.absolutePath();

        QList<std::tuple<QString, QString>> data{};

        QDirIterator it(inputAbsolutePath, QDir::Filter::Files);
        while (it.hasNext())
        {
            auto file = it.nextFileInfo();
            auto fileName = file.fileName();
            auto inputFilePath = inputDir.absoluteFilePath(fileName);

            data.append(std::make_tuple(fileName, inputFilePath));
        }
        return data;
    }
}

TestSuite ParserTestsSuite()
{
    TestSuite suite{};

    suite.add(QString("Examples"), Examples, Examples_Data);
    suite.add(QString("FileTests"), FileTests, FileTests_Data);

    return suite;
}
