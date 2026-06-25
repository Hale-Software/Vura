/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include <QTest>
#include <QObject>
#include <QList>
#include <QDebug>
#include <QString>
#include <QFile>

#include <libvura/data/VideoMarker.h>

class DataFileTest : public QObject {
	Q_OBJECT

private slots:
    void testSavingMarkers();
    void testLoadingAllMarkers();
    void testLoadingVideoMarkers();

};


void DataFileTest::testSavingMarkers()
{
    QList<VideoMarker> originalMarkers;

    originalMarkers.append({
        1,
        "testfile1.mp4",
        "marker1",
        "marker",
        "This is a test marker",
        5000
    });

    originalMarkers.append({
        2,
        "testfile1.mp4",
        "marker2",
        "marker",
        "This is a test marker",
        15000
    });

    originalMarkers.append({
        3,
        "testfile2.mp4",
        "marker3",
        "marker",
        "This is a test marker",
        25000
    });

    QString testFilePath = "test_project.vmrk";

    bool saveSuccess = VideoMarkerHandler::saveMarkersToFile(testFilePath, originalMarkers);

    QVERIFY(saveSuccess);
}

void DataFileTest::testLoadingAllMarkers()
{
    QList<VideoMarker> originalMarkers;

    originalMarkers.append({
        1,
        "testfile1.mp4",
        "marker1",
        "marker",
        "This is a test marker",
        5000
    });

    originalMarkers.append({
        2,
        "testfile1.mp4",
        "marker2",
        "marker",
        "This is a test marker",
        15000
    });

    originalMarkers.append({
        3,
        "testfile2.mp4",
        "marker3",
        "marker",
        "This is a test marker",
        25000
    });

    QString testFilePath = "test_project.vmrk";

    QList<VideoMarker> loadedMarkers = VideoMarkerHandler::loadAllMarkersFromFile(testFilePath);

    VideoMarker testMarker1 = loadedMarkers.at(0);
    VideoMarker testMarker2 = loadedMarkers.at(1);
    VideoMarker testMarker3 = loadedMarkers.at(2);

    QCOMPARE(testMarker1.id == originalMarkers.at(0).id, true);
    QCOMPARE(testMarker2.id == originalMarkers.at(1).id, true);
    QCOMPARE(testMarker3.id == originalMarkers.at(2).id, true);
    QCOMPARE(testMarker1.fileName == originalMarkers.at(0).fileName, true);
    QCOMPARE(testMarker2.fileName == originalMarkers.at(1).fileName, true);
    QCOMPARE(testMarker3.fileName == originalMarkers.at(2).fileName, true);
    QCOMPARE(testMarker1.markerName == originalMarkers.at(0).markerName, true);
    QCOMPARE(testMarker2.markerName == originalMarkers.at(1).markerName, true);
    QCOMPARE(testMarker3.markerName == originalMarkers.at(2).markerName, true);
    QCOMPARE(testMarker1.markerType == originalMarkers.at(0).markerType, true);
    QCOMPARE(testMarker2.markerType == originalMarkers.at(1).markerType, true);
    QCOMPARE(testMarker3.markerType == originalMarkers.at(2).markerType, true);
    QCOMPARE(testMarker1.comments == originalMarkers.at(0).comments, true);
    QCOMPARE(testMarker2.comments == originalMarkers.at(1).comments, true);
    QCOMPARE(testMarker3.comments == originalMarkers.at(2).comments, true);
    QCOMPARE(testMarker1.timestampMs == originalMarkers.at(0).timestampMs, true);
    QCOMPARE(testMarker2.timestampMs == originalMarkers.at(1).timestampMs, true);
    QCOMPARE(testMarker3.timestampMs == originalMarkers.at(2).timestampMs, true);
}

void DataFileTest::testLoadingVideoMarkers()
{
    QList<VideoMarker> originalMarkers;

    originalMarkers.append({
        1,
        "testfile1.mp4",
        "marker1",
        "marker",
        "This is a test marker",
        5000
    });

    originalMarkers.append({
        2,
        "testfile1.mp4",
        "marker2",
        "marker",
        "This is a test marker",
        15000
    });

    originalMarkers.append({
        3,
        "testfile2.mp4",
        "marker3",
        "marker",
        "This is a test marker",
        25000
    });

    QString testFilePath = "test_project.vmrk";

    QList<VideoMarker> video1Markers = VideoMarkerHandler::loadMarkersFromFile(testFilePath, "testfile1.mp4");
    QList<VideoMarker> video2Markers = VideoMarkerHandler::loadMarkersFromFile(testFilePath, "testfile2.mp4");

    QCOMPARE(video1Markers.size(), 2);
    QCOMPARE(video2Markers.size(), 1);
}


QTEST_MAIN(DataFileTest)

#include "tst_datafiletest.moc"
