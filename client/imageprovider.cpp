/**************************************************************************
 *                                                                        *
 * Copyright (C) 2016 Felix Rohrbach <kde@fxrh.de>                        *
 *                                                                        *
 * This program is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU General Public License            *
 * as published by the Free Software Foundation; either version 3         *
 * of the License, or (at your option) any later version.                 *
 *                                                                        *
 * This program is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 * GNU General Public License for more details.                           *
 *                                                                        *
 * You should have received a copy of the GNU General Public License      *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                        *
 **************************************************************************/

#include "imageprovider.h"

#include "lib/connection.h"
#include "jobs/mediathumbnailjob.h"

#include <QtCore/QWaitCondition>
#include <QtCore/QDebug>

using QMatrixClient::MediaThumbnailJob;

ImageProvider::ImageProvider(QMatrixClient::Connection* connection)
    : QQuickImageProvider(QQmlImageProviderBase::Pixmap,
                          QQmlImageProviderBase::ForceAsynchronousImageLoading)
    , m_connection(connection)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
    qRegisterMetaType<MediaThumbnailJob*>();
#endif
}

QPixmap ImageProvider::requestPixmap(const QString& id,
                                     QSize* pSize, const QSize& requestedSize)
{
    QMutexLocker locker(&m_mutex);
    qDebug() << "ImageProvider::requestPixmap:" << id;

    MediaThumbnailJob* job = nullptr;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QMetaObject::invokeMethod(m_connection,
        [=] { return m_connection->getThumbnail(id, requestedSize); },
        Qt::BlockingQueuedConnection, &job);
#else
    QMetaObject::invokeMethod(m_connection, "getThumbnail",
        Qt::BlockingQueuedConnection, Q_RETURN_ARG(MediaThumbnailJob*, job),
        Q_ARG(QUrl, id), Q_ARG(QSize, requestedSize));
#endif
    if (!job)
    {
        qDebug() << "ImageProvider: failed to send a request";
        return {};
    }
    QPixmap result;
    {
        QWaitCondition condition; // The most compact way to block on a signal
        job->connect(job, &MediaThumbnailJob::finished, job, [&] {
            result = job->thumbnail();
            condition.wakeAll();
        });
        condition.wait(&m_mutex);
    }

    if( pSize != nullptr )
        *pSize = result.size();

    return result;
}

void ImageProvider::setConnection(QMatrixClient::Connection* connection)
{
    QMutexLocker locker(&m_mutex);

    m_connection = connection;
}
