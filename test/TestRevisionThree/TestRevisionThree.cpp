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
#include "SourceMap/RevisionThree.h"

#include <QObject>
#include <QString>
#include <QtTest>

using namespace SourceMap;

class TestRevisionThree : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void storeAndRestore();
};

namespace {

#define SOURCE_ONE "sourceOne"
#define SOURCE_TWO "sourceTwo"
#define GENERATED "generated"
#define SYMBOL "symbol"

EntryList<> buildEntries()
{
    using Entry = SourceMap::Entry<>;

    return {{
        Entry{{1,1}},
        Entry{{1,10},{SOURCE_ONE, {1,1}}},
        Entry{{1,15}},
        Entry{{2,10},{SOURCE_TWO, {1,1}}, SYMBOL},
        Entry{{2,1}},
        Entry{{2,20}},
    }};
}

} // namespace

void TestRevisionThree::storeAndRestore()
{
    using Mapping = SourceMap::Mapping<>;
    using Data = Mapping::Data;

    auto m = Mapping{buildEntries()};

    RevisionThree r3;
    QCOMPARE(r3.version(), 3);

    r3.storeFile(GENERATED);
    QCOMPARE(r3.file(), QString(GENERATED));

    r3.encodeMappings(m);
    QCOMPARE(r3.sources().size(), 2);
    QCOMPARE(r3.names().size(), 1);

    auto bytes = r3.toJson();

    QJsonParseError error;
    auto rr = RevisionThree::fromJson(bytes, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QCOMPARE(rr.names(), r3.names());

    auto rm = Mapping{ rr.decodedMappings< Data >() };
    QCOMPARE(rm.data().entries().size(), m.data().entries().size());
    QCOMPARE(rm.originalNames(), m.originalNames());

    auto p1 = rm.findEntryByGenerated({2,10});
    QVERIFY(p1->isValid());
    QCOMPARE(p1->name, QString(SYMBOL));
}

QTEST_GUILESS_MAIN(TestRevisionThree)

#include "TestRevisionThree.moc"
