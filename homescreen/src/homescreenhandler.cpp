/*
 * Copyright (c) 2017, 2018, 2019 TOYOTA MOTOR CORPORATION
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

#include <QGuiApplication>
#include <QFileInfo>
#include "homescreenhandler.h"
#include <functional>
#include "hmi-debug.h"

#include <qpa/qplatformnativeinterface.h>

#define APPLAUNCH_DBUS_IFACE     "org.automotivelinux.AppLaunch"
#define APPLAUNCH_DBUS_OBJECT    "/org/automotivelinux/AppLaunch"

void* HomescreenHandler::myThis = 0;

HomescreenHandler::HomescreenHandler(Shell *_aglShell, ApplicationLauncher *launcher, QObject *parent) :
    QObject(parent),
    aglShell(_aglShell)
{
    mp_launcher = launcher;
    applaunch_iface = new org::automotivelinux::AppLaunch(APPLAUNCH_DBUS_IFACE, APPLAUNCH_DBUS_OBJECT,
                                                          QDBusConnection::sessionBus(), this);
}

HomescreenHandler::~HomescreenHandler()
{
#if 0
    if (mp_hs != NULL) {
        delete mp_hs;
    }
#endif
}

void HomescreenHandler::init(void)
{
#if 0
    mp_hs = new LibHomeScreen();
    mp_hs->init(port, token);
#endif
    myThis = this;

    /*
     * The "started" signal is received any time a start request is made to applaunchd,
     * and the application either starts successfully or is already running. This
     * effectively acts as a "switch to app X" action.
     */
    connect(applaunch_iface, SIGNAL(started(QString)), this, SLOT(appStarted(QString)));
    connect(applaunch_iface, SIGNAL(terminated(QString)), this, SLOT(appTerminated(QString)));

#if 0
    mp_hs->registerCallback(nullptr, HomescreenHandler::onRep_static);

    mp_hs->set_event_handler(LibHomeScreen::Event_OnScreenMessage, [this](json_object *object){
        const char *display_message = json_object_get_string(
            json_object_object_get(object, "display_message"));
        HMI_DEBUG("HomeScreen","set_event_handler Event_OnScreenMessage display_message = %s", display_message);
    });

    // should be handled in the top panel
    mp_hs->set_event_handler(LibHomeScreen::Event_ShowNotification,[this](json_object *object){
       json_object *p_obj = json_object_object_get(object, "parameter");
       const char *icon = json_object_get_string(
                   json_object_object_get(p_obj, "icon"));
       const char *text = json_object_get_string(
                   json_object_object_get(p_obj, "text"));
       const char *app_id = json_object_get_string(
                   json_object_object_get(p_obj, "caller"));
       HMI_DEBUG("HomeScreen","Event_ShowNotification icon=%s, text=%s, caller=%s", icon, text, app_id);
       QFileInfo icon_file(icon);
       QString icon_path;
       if (icon_file.isFile() && icon_file.exists()) {
           icon_path = QString(QLatin1String(icon));
       } else {
           icon_path = "./images/Utility_Logo_Grey-01.svg";
       }

       emit showNotification(QString(QLatin1String(app_id)), icon_path, QString(QLatin1String(text)));
    });

    // should be handled in the bottom panel
    mp_hs->set_event_handler(LibHomeScreen::Event_ShowInformation,[this](json_object *object){
       json_object *p_obj = json_object_object_get(object, "parameter");
       const char *info = json_object_get_string(
                   json_object_object_get(p_obj, "info"));

       emit showInformation(QString(QLatin1String(info)));
    });
#endif
}

static struct wl_output *
getWlOutput(QPlatformNativeInterface *native, QScreen *screen)
{
	void *output = native->nativeResourceForScreen("output", screen);
	return static_cast<struct ::wl_output*>(output);
}

void HomescreenHandler::tapShortcut(QString application_id)
{
	HMI_DEBUG("HomeScreen","tapShortcut %s", application_id.toStdString().c_str());
#if 0
	struct json_object* j_json = json_object_new_object();
	struct json_object* value;

	struct agl_shell *agl_shell = aglShell->shell.get();
	QPlatformNativeInterface *native = qApp->platformNativeInterface();
	struct wl_output *output = getWlOutput(native, qApp->screens().first());

	value = json_object_new_string("normal.full");
	json_object_object_add(j_json, "area", value);

	mp_hs->showWindow(application_id.toStdString().c_str(), j_json);

	// this works (and it is redundant the first time), due to the default
	// policy engine installed which actives the application, when starting
	// the first time. Later calls to HomescreenHandler::tapShortcut will
	// require calling 'agl_shell_activate_app'
	agl_shell_activate_app(agl_shell, application_id.toStdString().c_str(), output);

#endif

    if (mp_launcher) {
        mp_launcher->setCurrent(application_id);
    }
    appStarted(application_id);
}

#if 0
void HomescreenHandler::onRep_static(struct json_object* reply_contents)
{
    static_cast<HomescreenHandler*>(HomescreenHandler::myThis)->onRep(reply_contents);
}

void HomescreenHandler::onEv_static(const string& event, struct json_object* event_contents)
{
    static_cast<HomescreenHandler*>(HomescreenHandler::myThis)->onEv(event, event_contents);
}

void HomescreenHandler::onRep(struct json_object* reply_contents)
{
    const char* str = json_object_to_json_string(reply_contents);
    HMI_DEBUG("HomeScreen","HomeScreen onReply %s", str);
}

void HomescreenHandler::onEv(const string& event, struct json_object* event_contents)
{
    const char* str = json_object_to_json_string(event_contents);
    HMI_DEBUG("HomeScreen","HomeScreen onEv %s, contents: %s", event.c_str(), str);

    if (event.compare("homescreen/on_screen_message") == 0) {
        struct json_object *json_data = json_object_object_get(event_contents, "data");
        struct json_object *json_display_message = json_object_object_get(json_data, "display_message");
        const char* display_message = json_object_get_string(json_display_message);

        HMI_DEBUG("HomeScreen","display_message = %s", display_message);
    }
}
#endif

void HomescreenHandler::appStarted(const QString& application_id)
{
    struct agl_shell *agl_shell = aglShell->shell.get();
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    struct wl_output *output = getWlOutput(native, qApp->screens().first());

    HMI_DEBUG("HomeScreen", "Activating application %s", application_id.toStdString().c_str());
    agl_shell_activate_app(agl_shell, application_id.toStdString().c_str(), output);
}

void HomescreenHandler::appTerminated(const QString& application_id)
{
    HMI_DEBUG("HomeScreen", "Application %s terminated, activating launcher", application_id.toStdString().c_str());
    appStarted("launcher");
}
