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

#ifndef HOMESCREENCONTROLINTERFACE_H
#define HOMESCREENCONTROLINTERFACE_H

#include <QObject>
#include "include/homescreen.hpp"
#include "homescreen_adaptor.h"

class HomeScreenControlInterface : public QObject
{
    Q_OBJECT
public:
    explicit HomeScreenControlInterface(QObject *parent = 0);

signals:
    void newRequestsToBeVisibleApp(int pid);

    QList<int> newRequestGetAllSurfacesOfProcess(int pid);
    int newRequestGetSurfaceStatus(int surfaceId);
    void newRequestRenderSurfaceToArea(int surfaceId, int layoutArea);
    bool newRequestRenderSurfaceToAreaAllowed(int surfaceId, int layoutArea);
    void newRequestSurfaceIdToFullScreen(int surfaceId);

//from homescreen_adaptor.h
public Q_SLOTS: // METHODS
    QList<int> getAllSurfacesOfProcess(int pid);
    int getSurfaceStatus(int surfaceId);
    void hardKeyPressed(int key);
    void renderSurfaceToArea(int surfaceId, int layoutArea);
    bool renderAppToAreaAllowed(int appCategory, int layoutArea);
    void requestSurfaceIdToFullScreen(int surfaceId);

private:
    HomescreenAdaptor *mp_homeScreenAdaptor;
};

#endif // HOMESCREENCONTROLINTERFACE_H
