/* SourceMap-Qt
 * (C) Copyright 2014 HicknHack Software GmbH
 *
 * The original code can be found at:
 *     https://github.com/hicknhack-software/SourceMap-Qt
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "SourceMap/Entry.h"

#include <QObject>
#include <QString>
#include <QtTest>

class TestStructs : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void position();
    void filePosition();
    void entry();
};

void TestStructs::position()
{
    using namespace SourceMap;

    auto p1 = Position{};
    QCOMPARE(p1.isValid(), false);

    auto p2 = Position{3,4};
    QCOMPARE(p2.isValid(), true);
    QCOMPARE(p2.line, 3);
    QCOMPARE(p2.column, 4);

    QVERIFY(p1 != p2);
    QVERIFY(p2 == Position(3,4));
}

#define TEST_STR1 "hello"

void TestStructs::filePosition()
{
    using namespace SourceMap;

    auto p1 = FilePosition{};
    QCOMPARE(p1.isValid(), false);

    auto p2 = FilePosition{TEST_STR1, {3,4}};
    QCOMPARE(p2.isValid(), true);
    QCOMPARE(p2.name, QString(TEST_STR1));
    QCOMPARE(p2.line, 3);
    QCOMPARE(p2.column, 4);
}

void TestStructs::entry()
{
    using namespace SourceMap;

    using Entry = SourceMap::Entry<>;

    auto p1 = Entry{};
    QCOMPARE(p1.isValid(), false);

    auto p2 = Entry{{3,4}};
    QCOMPARE(p2.isValid(), true);
    QCOMPARE(p2.generated, Position(3,4));

    auto p3 = Entry{{3,4}, {TEST_STR1, {6,4}}};
    QCOMPARE(p3.isValid(), true);
    QCOMPARE(p3.generated, Position(3,4));
    QCOMPARE(p3.original, FilePosition(TEST_STR1, {6,4}));

    auto p4 = Entry{{3,4}, {TEST_STR1, {6,4}}, TEST_STR1};
    QCOMPARE(p4.isValid(), true);
    QCOMPARE(p4.generated, Position(3,4));
    QCOMPARE(p4.original, FilePosition(TEST_STR1, {6,4}));
    QCOMPARE(p4.name, QString(TEST_STR1));
}

QTEST_GUILESS_MAIN(TestStructs)

#include "TestStructs.moc"
