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

#pragma once

#include <QThread>
#include <QString>
#include <QDebug>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class RemuxerThread : public QThread {
    Q_OBJECT

public:
    RemuxerThread(const QString &inputFile, const QString &outputFile, QObject *parent = nullptr);
    ~RemuxerThread() override;

    void cancel();

signals:
    void progressUpdated(int percentage);
    void conversionFinished(bool success, const QString &message);

protected:
    void run() override;

private:
    QString m_inputFile;
    QString m_outputFile;
    bool m_cancelRequested = false;

};
