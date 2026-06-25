#include <QTest>
#include <QObject>
#include <QList>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QColor>

#include <libvura/data/VideoMarker.h>

class CoreTest : public QObject {
	Q_OBJECT

public:

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
};

void CoreTest::initTestCase() {}
void CoreTest::cleanupTestCase() {}
void CoreTest::init() {}
void CoreTest::cleanup() {}

void CoreTest::testVideoMarkerFile()
{
    qDebug() << "--- Starting Vura Marker Test ---";

    // 1. Create some dummy markers
    QList<VideoMarker> originalMarkers;

    originalMarkers.append({
        5000,                           // 5 seconds in
        "Intro Ends",
        QColor(255, 0, 0)               // Red
    });

    originalMarkers.append({
        65500,                          // 1 minute, 5.5 seconds in
        "Beat Drop / Action Starts",
        QColor(0, 255, 0)               // Green
    });

    originalMarkers.append({
        120000,                         // 2 minutes in
        "Credits Roll",
        QColor(0, 0, 255)               // Blue
    });

    QString testFilePath = "test_project.vmrk";

    // 2. Save the markers to our custom binary format
    qDebug() << "Saving" << originalMarkers.size() << "markers to" << testFilePath << "...";
    bool saveSuccess = VideoMarkers::saveMarkersToFile(testFilePath, originalMarkers);

    if (saveSuccess) {
        qDebug() << "Save Successful!";
    } else {
        qDebug() << "Save Failed!";
        return;
    }

    // 3. Load the markers back from the file
    qDebug() << "Loading markers from" << testFilePath << "...";
    QList<VideoMarker> loadedMarkers = VideoMarkers::loadMarkersFromFile(testFilePath);

    // 4. Verify the results
    qDebug() << "Loaded" << loadedMarkers.size() << "markers. Verifying data:";

    for (int i = 0; i < loadedMarkers.size(); ++i) {
        const VideoMarker& m = loadedMarkers.at(i);
        qDebug() << "Marker" << i + 1 << ":"
                 << "\n  Timestamp:" << m.timestampMs << "ms"
                 << "\n  Title:    " << m.title
                 << "\n  Color:    " << m.color.name(); // .name() prints the hex code (e.g., #ff0000)
    }

    qDebug() << "--- Test Complete ---";
}


QTEST_MAIN(CoreTest)

#include "tst_coretest.moc"
