#include <libvura/media/media-controller.h>
#include <libvura/media/playlist.h>
#include <libvura/media/resume-store.h>
#include <libvura/media/engine-factory.h>
#include <libvura/media/null-engine.h>

#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest>

using namespace media;

namespace {

QUrl simulated(int durationMs = 180'000, const QString &extra = {})
{
    QString url = QStringLiteral("null:///clip%1?duration=%2")
                          .arg(QString::number(durationMs))
                          .arg(durationMs);
    if (!extra.isEmpty())
        url += QLatin1Char('&') + extra;
    return QUrl(url);
}

} // namespace

class PlayerTest : public QObject
{
    Q_OBJECT

private slots:
    // ---- engine contract --------------------------------------------------

    void engineReportsLoadedState();
    void engineClampsSeeks();
    void engineReachesEndOfMedia();
    void engineSurfacesErrors();
    void engineRejectsUnknownTrackIds();
    void engineIdsAreNotIndices();

    // ---- playlist ---------------------------------------------------------

    void playlistAdvances();
    void playlistRepeatOneLoopsButNextDoesNot();
    void playlistRepeatAllWraps();
    void playlistShuffleCoversEveryItem();
    void playlistTracksCurrentAcrossRemoval();
    void playlistMoveKeepsCurrent();

    // ---- resume -----------------------------------------------------------

    void resumeIgnoresEdgesAndShortMedia();
    void resumeRoundTripsThroughDisk();

    // ---- controller -------------------------------------------------------

    void controllerThrottlesPositionUpdates();
    void controllerAdvancesAtEndOfMedia();
    void controllerSkipsUnplayableItems();
    void controllerSwapCarriesStateAcross();
};

// ---------------------------------------------------------------------------

void PlayerTest::engineReportsLoadedState()
{
    NullEngine engine;
    QSignalSpy statusSpy(&engine, &Engine::mediaStatusChanged);

    engine.setSource(simulated(60'000));

    QCOMPARE(engine.duration(), 60'000);
    QVERIFY(engine.isSeekable());
    QCOMPARE(engine.mediaStatus(), MediaStatus::Buffered);
    QVERIFY(statusSpy.count() >= 2); // Loading, then Loaded/Buffered.

    engine.play();
    QCOMPARE(engine.playbackState(), PlaybackState::Playing);

    engine.pause();
    QCOMPARE(engine.playbackState(), PlaybackState::Paused);

    engine.stop();
    QCOMPARE(engine.playbackState(), PlaybackState::Stopped);
    QCOMPARE(engine.position(), 0);
}

void PlayerTest::engineClampsSeeks()
{
    NullEngine engine;
    engine.setSource(simulated(10'000));

    engine.seek(-5'000);
    QCOMPARE(engine.position(), 0);

    engine.seek(999'999);
    QCOMPARE(engine.position(), 10'000);

    engine.seek(4'000);
    engine.seekRelative(3'000);
    QCOMPARE(engine.position(), 7'000);

    engine.seekRelative(-100'000);
    QCOMPARE(engine.position(), 0);
}

void PlayerTest::engineReachesEndOfMedia()
{
    NullEngine engine;
    engine.setSource(simulated(5'000));
    engine.play();

    engine.advance(4'900);
    QCOMPARE(engine.mediaStatus(), MediaStatus::Buffered);

    engine.advance(200);
    QCOMPARE(engine.mediaStatus(), MediaStatus::EndOfMedia);
    QCOMPARE(engine.playbackState(), PlaybackState::Stopped);
    QCOMPARE(engine.position(), 5'000);
}

void PlayerTest::engineSurfacesErrors()
{
    NullEngine engine;
    QSignalSpy errorSpy(&engine, &Engine::errorOccurred);

    engine.setSource(QUrl(QStringLiteral("null:///broken?fail=notfound")));

    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(errorSpy.first().at(0).value<ErrorKind>(), ErrorKind::NotFound);
    QCOMPARE(engine.mediaStatus(), MediaStatus::Invalid);
    QVERIFY(!isRecoverable(ErrorKind::NotFound));
    QVERIFY(isRecoverable(ErrorKind::NetworkFailure));
}

void PlayerTest::engineRejectsUnknownTrackIds()
{
    NullEngine engine;
    engine.setSource(simulated(60'000, QStringLiteral("audio=2&subs=2")));

    const QString original = engine.activeTrack(TrackType::Audio);
    engine.selectTrack(TrackType::Audio, QStringLiteral("nonsense"));
    QCOMPARE(engine.activeTrack(TrackType::Audio), original);

    const auto tracks = engine.tracks(TrackType::Audio);
    QCOMPARE(tracks.size(), 2);
    engine.selectTrack(TrackType::Audio, tracks.at(1).id);
    QCOMPARE(engine.activeTrack(TrackType::Audio), tracks.at(1).id);

    // Subtitles can be switched off; audio cannot be, by this engine.
    engine.selectTrack(TrackType::Subtitle, {});
    QVERIFY(engine.activeTrack(TrackType::Subtitle).isEmpty());
}

void PlayerTest::engineIdsAreNotIndices()
{
    // Guards the reason TrackInfo::id is a string: real backends hand out
    // sparse ids, so anything that assumes id == index breaks here rather
    // than in production.
    NullEngine engine;
    engine.setSource(simulated(60'000, QStringLiteral("audio=3")));

    const auto tracks = engine.tracks(TrackType::Audio);
    QCOMPARE(tracks.size(), 3);
    for (int i = 0; i < tracks.size(); ++i)
        QVERIFY(tracks.at(i).id != QString::number(i));
}

// ---------------------------------------------------------------------------

void PlayerTest::playlistAdvances()
{
    Playlist playlist;
    playlist.append({simulated(1000), simulated(2000), simulated(3000)});
    QCOMPARE(playlist.count(), 3);

    playlist.setCurrentIndex(0);
    QCOMPARE(playlist.nextIndex(), 1);
    playlist.setCurrentIndex(2);
    QCOMPARE(playlist.nextIndex(), -1);
    QCOMPARE(playlist.previousIndex(), 1);
}

void PlayerTest::playlistRepeatOneLoopsButNextDoesNot()
{
    Playlist playlist;
    playlist.append({simulated(1000), simulated(2000)});
    playlist.setCurrentIndex(0);
    playlist.setRepeatMode(Playlist::RepeatOne);

    // Track ended: repeat it.
    QCOMPARE(playlist.nextIndex(false), 0);
    // User pressed Next: honour that instead of trapping them.
    QCOMPARE(playlist.nextIndex(true), 1);
}

void PlayerTest::playlistRepeatAllWraps()
{
    Playlist playlist;
    playlist.append({simulated(1000), simulated(2000)});
    playlist.setRepeatMode(Playlist::RepeatAll);

    playlist.setCurrentIndex(1);
    QCOMPARE(playlist.nextIndex(), 0);
    playlist.setCurrentIndex(0);
    QCOMPARE(playlist.previousIndex(), 1);
}

void PlayerTest::playlistShuffleCoversEveryItem()
{
    Playlist playlist;
    QList<QUrl> urls;
    for (int i = 0; i < 20; ++i)
        urls.append(simulated(1000 + i));
    playlist.append(urls);

    playlist.setCurrentIndex(0);
    playlist.setShuffled(true);

    // A permutation, not a random pick each time: every item must appear
    // exactly once before anything repeats.
    QSet<int> seen{playlist.currentIndex()};
    for (int i = 0; i < 19; ++i) {
        const int next = playlist.nextIndex();
        QVERIFY2(next >= 0, "shuffle exhausted the queue early");
        QVERIFY2(!seen.contains(next), "shuffle repeated an item before covering the queue");
        seen.insert(next);
        playlist.setCurrentIndex(next);
    }
    QCOMPARE(seen.size(), 20);
}

void PlayerTest::playlistTracksCurrentAcrossRemoval()
{
    Playlist playlist;
    playlist.append({simulated(1), simulated(2), simulated(3), simulated(4)});
    playlist.setCurrentIndex(2);

    playlist.removeRows(0, 1);
    QCOMPARE(playlist.currentIndex(), 1); // Shifted down, still the same item.

    playlist.setCurrentIndex(1);
    playlist.removeRows(1, 1);
    QCOMPARE(playlist.currentIndex(), -1); // The playing item itself went away.
}

void PlayerTest::playlistMoveKeepsCurrent()
{
    Playlist playlist;
    playlist.append({simulated(1), simulated(2), simulated(3)});
    playlist.setCurrentIndex(0);

    playlist.move(0, 2);
    QCOMPARE(playlist.currentIndex(), 2);

    playlist.setCurrentIndex(2);
    playlist.move(0, 1);
    QCOMPARE(playlist.currentIndex(), 2);
}

// ---------------------------------------------------------------------------

void PlayerTest::resumeIgnoresEdgesAndShortMedia()
{
    QTemporaryDir dir;
    ResumeStore store(nullptr, dir.filePath(QStringLiteral("resume.json")));

    const QUrl url(QStringLiteral("file:///movie.mkv"));

    store.remember(url, 30'000, 60'000); // Too short overall.
    QCOMPARE(store.positionFor(url), 0);

    store.remember(url, 1'000, 600'000); // Barely started.
    QCOMPARE(store.positionFor(url), 0);

    store.remember(url, 599'000, 600'000); // Effectively finished.
    QCOMPARE(store.positionFor(url), 0);

    store.remember(url, 300'000, 600'000);
    QCOMPARE(store.positionFor(url), 300'000);

    store.forget(url);
    QCOMPARE(store.positionFor(url), 0);
}

void PlayerTest::resumeRoundTripsThroughDisk()
{
    QTemporaryDir dir;
    const QString path = dir.filePath(QStringLiteral("resume.json"));
    const QUrl url(QStringLiteral("file:///show.mkv"));

    {
        ResumeStore store(nullptr, path);
        store.remember(url, 120'000, 600'000);
        store.save();
    }
    {
        ResumeStore reopened(nullptr, path);
        QCOMPARE(reopened.positionFor(url), 120'000);
    }
}

// ---------------------------------------------------------------------------

void PlayerTest::controllerThrottlesPositionUpdates()
{
    MediaController controller;
    QVERIFY(controller.setBackend(Backend::Null));

    auto *engine = qobject_cast<NullEngine *>(controller.engine());
    QVERIFY(engine);

    controller.openUrls({simulated(60'000)});
    QTRY_COMPARE(controller.playbackState(), PlaybackState::Playing);

    QSignalSpy positionSpy(&controller, &MediaController::positionChanged);

    // Ten 50 ms steps: half a second of playback. With a 250 ms threshold
    // the UI should hear about it twice, not ten times.
    for (int i = 0; i < 10; ++i)
        engine->advance(50);

    QVERIFY2(positionSpy.count() <= 3,
             qPrintable(QStringLiteral("expected throttling, got %1 updates")
                                .arg(positionSpy.count())));
    QVERIFY(positionSpy.count() >= 1);
}

void PlayerTest::controllerAdvancesAtEndOfMedia()
{
    MediaController controller;
    QVERIFY(controller.setBackend(Backend::Null));

    auto *engine = qobject_cast<NullEngine *>(controller.engine());
    QVERIFY(engine);

    controller.openUrls({simulated(5'000), simulated(7'000)});
    QTRY_COMPARE(controller.playlist()->currentIndex(), 0);
    QTRY_COMPARE(controller.playbackState(), PlaybackState::Playing);

    engine->advance(5'000);

    QTRY_COMPARE(controller.playlist()->currentIndex(), 1);
    QTRY_COMPARE(controller.duration(), 7'000);
}

void PlayerTest::controllerSkipsUnplayableItems()
{
    MediaController controller;
    QVERIFY(controller.setBackend(Backend::Null));

    controller.openUrls({QUrl(QStringLiteral("null:///bad?fail=codec")), simulated(9'000)});

    // A missing codec is terminal, so the queue moves on rather than stalling.
    QTRY_COMPARE(controller.playlist()->currentIndex(), 1);
    QTRY_COMPARE(controller.duration(), 9'000);
}

void PlayerTest::controllerSwapCarriesStateAcross()
{
    MediaController controller;
    QVERIFY(controller.setBackend(Backend::Null));

    auto *engine = qobject_cast<NullEngine *>(controller.engine());
    QVERIFY(engine);

    controller.openUrls({simulated(120'000)});
    QTRY_COMPARE(controller.playbackState(), PlaybackState::Playing);

    controller.setVolume(0.4);
    engine->advance(30'000);
    const Msec before = controller.position();
    QVERIFY(before >= 30'000);

    QSignalSpy backendSpy(&controller, &MediaController::backendChanged);

    // Swapping to the same backend still exercises the whole capture and
    // restore path, which is the point: it is how the abstraction gets
    // tested without needing two real decoders present.
    QVERIFY(controller.setBackend(Backend::Null));
    QCOMPARE(backendSpy.count(), 1);

    QTRY_COMPARE(controller.duration(), 120'000);
    QTRY_VERIFY(controller.position() >= before - 1000);
    QTRY_COMPARE(controller.playbackState(), PlaybackState::Playing);
    QVERIFY(qAbs(controller.volume() - 0.4) < 0.01);
}

QTEST_MAIN(PlayerTest)
#include "tst_player.moc"
