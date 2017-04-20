/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016, 2017 Mentor Graphics Development (Deutschland) GmbH
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

#include "applicationmodel.h"
#include "appinfo.h"

#include <QtCore/QDebug>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

class ApplicationModel::Private
{
public:
    Private(ApplicationModel *parent);

private:
    ApplicationModel *q;
public:
    QDBusInterface proxy;
    QList<AppInfo> data;
};

namespace {
    // This is a disgrace, shouldn't we be having a defined way to know which icon (if it is not given by the getAvailableApps() reply)?
    QString get_icon_name(AppInfo const &i)
    {
        QString icon = i.iconPath().isEmpty() ? i.id().split("@").front() : i.iconPath();
        if (icon == "hvac" || icon == "poi") {
            icon = icon.toUpper();
        } else if (icon == "mediaplayer") {
            icon = "Multimedia";
        } else {
            icon[0] = icon[0].toUpper();
        }
        return icon;
    }
}

ApplicationModel::Private::Private(ApplicationModel *parent)
    : q(parent)
    , proxy(QStringLiteral("org.agl.homescreenappframeworkbinder"), QStringLiteral("/AppFramework"), QStringLiteral("org.agl.appframework"), QDBusConnection::sessionBus())
{
    QDBusReply<QList<AppInfo>> reply = proxy.call("getAvailableApps");
    if (reply.isValid()) {
        // FIXME: Is the order from dbus the one we want to use?!
        for (auto const &i: reply.value()) {
            auto const name = i.name().split(" ").front().toUpper();
            auto const icon = get_icon_name(i);
            data.append(AppInfo(icon, name, i.id()));
        }
    } else {
        qDebug() << "getAvailableApps() reply is INVALID!";
    }
}

ApplicationModel::ApplicationModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
}

ApplicationModel::~ApplicationModel()
{
    delete d;
}

int ApplicationModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return d->data.count();
}

QVariant ApplicationModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    if (!index.isValid())
        return ret;

    switch (role) {
    case Qt::DecorationRole:
        ret = d->data[index.row()].iconPath();
        break;
    case Qt::DisplayRole:
        ret = d->data[index.row()].name();
        break;
    case Qt::UserRole:
        ret = d->data[index.row()].id();
        break;
    default:
        break;
    }

    return ret;
}

QHash<int, QByteArray> ApplicationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DecorationRole] = "icon";
    roles[Qt::DisplayRole] = "name";
    roles[Qt::UserRole] = "id";
    return roles;
}
