#pragma once

#include <QtGlobal>
#include <QAtomicInt>
#include <QMutex>
#include <QWaitCondition>
#include <QThreadStorage>
#include <QThread>

namespace CallOnce {
enum ECallOnce {
	CO_Request,
	CO_InProgress,
	CO_Finished
    };

Q_GLOBAL_STATIC(QThreadStorage<QAtomicInt*>, once_flag)
}

template <class Function>
inline static void qCallOnce(Function func, QBasicAtomicInt& flag)
{
	using namespace CallOnce;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	int protectFlag = flag.fetchAndStoreAcquire(flag);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	int protectFlag = flag.fetchAndStoreAcquire(flag.loadRelaxed());
#else
	int protectFlag = flag.fetchAndStoreAcquire(flag.load());
#endif

	if (protectFlag == CO_Finished)
		return;
	if (protectFlag == CO_Request && flag.testAndSetRelaxed(protectFlag,
							       CO_InProgress)) {
		func();
		flag.fetchAndStoreRelease(CO_Finished);
							       }
	else {
		do {
			QThread::yieldCurrentThread();
		}
		while (!flag.testAndSetAcquire(CO_Finished, CO_Finished));
	}
}

template <class Function>
inline static void qCallOncePerThread(Function func)
{
	using namespace CallOnce;
	if (!CallOnce::once_flag()->hasLocalData()) { // qualifie with CallOnce:: due to conflict on MacOS X
		CallOnce::once_flag()->setLocalData(new QAtomicInt(CO_Request));
		qCallOnce(func, *CallOnce::once_flag()->localData());
	}
}
