/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2017, 2018 TOYOTA MOTOR CORPORATION
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "statusbarmodel.h"
#include "statusbarserver.h"

#include <QtDBus/QDBusConnection>

class StatusBarModel::Private
{
public:
    Private(StatusBarModel *parent);

private:
    StatusBarModel *q;
public:
    StatusBarServer server;
    QString iconList[StatusBarServer::SupportedCount];
};

StatusBarModel::Private::Private(StatusBarModel *parent)
    : q(parent)
{
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/StatusBar", &server);
    dbus.registerService("org.agl.homescreen");
    connect(&server, &StatusBarServer::statusIconChanged, [&](int placeholderIndex, const QString &icon) {
        if (placeholderIndex < 0 || StatusBarServer::SupportedCount <= placeholderIndex) return;
        if (iconList[placeholderIndex] == icon) return;
        iconList[placeholderIndex] = icon;
        emit q->dataChanged(q->index(placeholderIndex), q->index(placeholderIndex));
    });
    for (int i = 0; i < StatusBarServer::SupportedCount; i++) {
        iconList[i] = server.getStatusIcon(i);
    }
}

StatusBarModel::StatusBarModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
}

StatusBarModel::~StatusBarModel()
{
    delete d;
}

int StatusBarModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // Delete bluetooth because use agl-service-bluetooth.
    return StatusBarServer::SupportedCount - 1;
}

QVariant StatusBarModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid())
        return ret;

    switch (role) {
    case Qt::DisplayRole:
        if (index.row() == 0){
            ret = d->iconList[StatusBarServer::StatusWifi];
        }else if (index.row() == 1){
            ret = d->iconList[StatusBarServer::StatusCellular];
        }
        break;
    default:
        break;
    }

    return ret;
}

QHash<int, QByteArray> StatusBarModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "icon";
    return roles;
}
