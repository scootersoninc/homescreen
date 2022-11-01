/*
 * Copyright © 2019, 2022 Collabora Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <QGuiApplication>
#include <QDebug>
#include "shell.h"
// defined by meson build file
#include QT_QPA_HEADER
#include <stdio.h>

static struct wl_output *
getWlOutput(QPlatformNativeInterface *native, QScreen *screen)
{
	void *output = native->nativeResourceForScreen("output", screen);
	return static_cast<struct ::wl_output*>(output);
}

void Shell::activate_app(QWindow *win, const QString &app_id)
{
    QPlatformNativeInterface *native = qApp->platformNativeInterface();
    QScreen *screen = win->screen();

    struct wl_output *output = getWlOutput(native, screen);

    qDebug() << "++ activating app_id " << app_id.toStdString().c_str();

    agl_shell_activate_app(this->shell.get(),
                           app_id.toStdString().c_str(),
                           output);
}

void Shell::set_activate_region(struct wl_output *output, int32_t x, int32_t y,
				int32_t width, int32_t height)
{
#ifdef AGL_SHELL_SET_ACTIVATE_REGION_SINCE_VERSION
	agl_shell_set_activate_region(this->shell.get(), output, x, y, width, height);
#endif
}
