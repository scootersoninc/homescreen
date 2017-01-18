/*
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

#include "homescreencontrolinterface.h"

HomeScreenControlInterface::HomeScreenControlInterface(QObject *parent) :
    QObject(parent),
    mp_homeScreenAdaptor(0),
    mp_dBusAppFrameworkProxy()
{
    // publish dbus homescreen interface
    mp_homeScreenAdaptor = new HomescreenAdaptor((QObject*)this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/HomeScreen", this);
    dbus.registerService("org.agl.homescreen");

    qDebug("D-Bus: connect to org.agl.homescreenappframeworkbindertizen /AppFramework");
    mp_dBusAppFrameworkProxy = new org::agl::appframework("org.agl.homescreenappframeworkbindertizen",
                                              "/AppFramework",
                                              QDBusConnection::sessionBus(),
                                              0);
}

HomeScreenControlInterface::~HomeScreenControlInterface()
{
    delete mp_dBusAppFrameworkProxy;
    delete mp_homeScreenAdaptor;
}

QList<int> HomeScreenControlInterface::getAllSurfacesOfProcess(int pid)
{
    qDebug("getAllSurfacesOfProcess %d", pid);
    return newRequestGetAllSurfacesOfProcess(pid);
}

int HomeScreenControlInterface::getSurfaceStatus(int surfaceId)
{
    qDebug("getSurfaceStatus %d", surfaceId);
    return newRequestGetSurfaceStatus(surfaceId);
}

void HomeScreenControlInterface::hardKeyPressed(int key)
{
    int pid = -1;

    switch (key)
    {
    case InputEvent::HARDKEY_NAV:
        qDebug("hardKeyPressed NAV key pressed!");
        pid = mp_dBusAppFrameworkProxy->launchApp("navigation@0.1");
        qDebug("pid: %d", pid);
        emit newRequestsToBeVisibleApp(pid);
        break;
    case InputEvent::HARDKEY_MEDIA:
        qDebug("hardKeyPressed MEDIA key pressed!");
        pid = mp_dBusAppFrameworkProxy->launchApp("media@0.1");
        qDebug("pid: %d", pid);
        emit newRequestsToBeVisibleApp(pid);
        break;
    default:
        qDebug("hardKeyPressed %d", key);
        break;
    }
}

void HomeScreenControlInterface::renderSurfaceToArea(int surfaceId, int layoutArea)
{
    qDebug("renderSurfaceToArea %d %d", surfaceId, layoutArea);
    emit newRequestRenderSurfaceToArea(surfaceId, layoutArea);
}

bool HomeScreenControlInterface::renderAppToAreaAllowed(int appCategory, int layoutArea)
{
    qDebug("renderAppToAreaAllowed %d %d", appCategory, layoutArea);
    return true; //TODO: ask policy manager
}

void HomeScreenControlInterface::requestSurfaceIdToFullScreen(int surfaceId)
{
    qDebug("requestSurfaceIdToFullScreen %d", surfaceId);
    emit newRequestSurfaceIdToFullScreen(surfaceId);
}
