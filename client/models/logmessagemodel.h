/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef LOGMESSAGEMODEL_H
#define LOGMESSAGEMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QModelIndex>

namespace QMatrixClient
{
    class Room;
    class LogMessage;
}

class LogMessageModel: public QAbstractListModel
{
        Q_OBJECT
    public:
        LogMessageModel(QObject* parent=0);
        virtual ~LogMessageModel();

        void changeRoom(QMatrixClient::Room* room);

        //override QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
        //override QModelIndex parent(const QModelIndex& index) const;
        int rowCount(const QModelIndex& parent) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    public slots:
        void newMessages(QList<QMatrixClient::LogMessage*> messages);

    private:
        QMatrixClient::Room* m_currentRoom;
        QList<QMatrixClient::LogMessage*> m_currentMessages;
};

#endif // LOGMESSAGEMODEL_H