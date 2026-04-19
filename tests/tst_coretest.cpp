#include <QTest>
#include <QCoreApplication>

// add necessary includes here

class CoreTest : public QObject {
	Q_OBJECT

public:
	CoreTest();
	~CoreTest() override;

private slots:
	void test_case1();
};

CoreTest::CoreTest() {}

CoreTest::~CoreTest() = default;

void CoreTest::test_case1() {}

QTEST_MAIN(CoreTest)

#include "tst_coretest.moc"
