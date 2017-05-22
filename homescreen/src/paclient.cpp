/*
 * Copyright (C) 2016,2017 Konsulko Group
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

#include "paclient.h"

#include <QtCore/QDebug>

PaClient::PaClient()
	: m_init(false), m_ml(nullptr), m_mlapi(nullptr), m_ctx(nullptr)
{
}

PaClient::~PaClient()
{
	if (m_init)
		close();
}

void PaClient::close()
{
	if (!m_init) return;
	pa_threaded_mainloop_stop(m_ml);
	pa_threaded_mainloop_free(m_ml);
	m_init = false;
}

static void set_sink_volume_cb(pa_context *c, int success, void *data)
{
	Q_UNUSED(data);

	if (!success)
		qWarning() << "PaClient: set sink volume: " <<
			pa_strerror(pa_context_errno(c));
}

void PaClient::incDecVolume(const int volume_delta)
{
	pa_operation *o;
	pa_context *c = context();
	pa_sink_info *i = getDefaultSinkInfo();

	if (volume_delta > 0)
		pa_cvolume_inc_clamp(&i->volume, volume_delta, 65536);
	else
		pa_cvolume_dec(&i->volume, abs(volume_delta));

	o = pa_context_set_sink_volume_by_index(c, i->index, &i->volume, set_sink_volume_cb, NULL);
	if (!o) {
		qWarning() << "PaClient: set sink #" << i->index <<
			" volume: " << pa_strerror(pa_context_errno(c));
		return;
	}
	pa_operation_unref(o);
}

void get_sink_info_change_cb(pa_context *c,
		const pa_sink_info *i,
		int eol,
		void *data)
{
	Q_UNUSED(c);
	Q_ASSERT(i);
	Q_ASSERT(data);

	if (eol) return;

	PaClient *self = reinterpret_cast<PaClient*>(data);
	pa_sink_info *si = self->getDefaultSinkInfo();
	if (i->index == si->index) {
		self->setDefaultSinkInfo(i);
		pa_cvolume *cvolume = &self->getDefaultSinkInfo()->volume;
		emit self->volumeExternallyChanged(pa_cvolume_avg(cvolume));
	}
}

void get_default_sink_info_cb(pa_context *c,
		const pa_sink_info *i,
		int eol,
		void *data)
{
	Q_UNUSED(c);
	Q_ASSERT(i);
	Q_ASSERT(data);

	if (eol) return;

	PaClient *self = reinterpret_cast<PaClient*>(data);
	self->setDefaultSinkInfo(i);
}

pa_sink_info *PaClient::getDefaultSinkInfo(void)
{
	return &m_default_sink_info;
}

void PaClient::setDefaultSinkInfo(const pa_sink_info *i)
{
	m_default_sink_info.index = i->index;
	m_default_sink_info.channel_map.channels = i->channel_map.channels;
	pa_cvolume *cvolume = &m_default_sink_info.volume;
	cvolume->channels = i->volume.channels;
	for (int chan = 0; chan < i->channel_map.channels; chan++) {
		cvolume->values[chan] = i->volume.values[chan];
	}
}

void get_server_info_cb(pa_context *c,
		const pa_server_info *i,
		void *data)
{
	pa_operation *o;
	o = pa_context_get_sink_info_by_name(c, i->default_sink_name, get_default_sink_info_cb, data);
	if (!o) {
		qWarning() << "PaClient: get sink info by name: " <<
			pa_strerror(pa_context_errno(c));
		return;
	}
}

void subscribe_cb(pa_context *c,
		pa_subscription_event_type_t type,
		uint32_t index,
		void *data)
{
	pa_operation *o;

	if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) != PA_SUBSCRIPTION_EVENT_CHANGE) {
		qWarning("PaClient: unhandled subscribe event operation");
		return;
	}

	switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
		case PA_SUBSCRIPTION_EVENT_SINK:
			o = pa_context_get_sink_info_by_index(c, index, get_sink_info_change_cb, data);
			if (!o) {
				qWarning() << "PaClient: get sink info by index: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			break;
		default:
			qWarning("PaClient: unhandled subscribe event facility");
	}
}

void context_state_cb(pa_context *c, void *data)
{
	pa_operation *o;
	PaClient *self = reinterpret_cast<PaClient*>(data);

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;
		case PA_CONTEXT_READY:
			o = pa_context_get_server_info(c, get_server_info_cb, data);
			if (!o) {
				qWarning() << "PaClient: get server info: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			pa_operation_unref(o);
			pa_context_set_subscribe_callback(c, subscribe_cb, data);
			o = pa_context_subscribe(c, (pa_subscription_mask_t)(PA_SUBSCRIPTION_MASK_SINK), NULL, NULL);
			if (!o) {
				qWarning() << "PaClient: subscribe: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			break;
		case PA_CONTEXT_TERMINATED:
			self->close();
			break;

		case PA_CONTEXT_FAILED:
		default:
			qCritical() << "PaClient: connection failed: " <<
				pa_strerror(pa_context_errno(c));
			self->close();
			break;
	}
}

void PaClient::init()
{
	m_ml = pa_threaded_mainloop_new();
	if (!m_ml) {
		qCritical("PaClient: failed to create mainloop");
		return;
	}

	pa_threaded_mainloop_set_name(m_ml, "PaClient mainloop");

	m_mlapi = pa_threaded_mainloop_get_api(m_ml);

	lock();

	m_ctx = pa_context_new(m_mlapi, "HomeScreen");
	if (!m_ctx) {
		qCritical("PaClient: failed to create context");
		unlock();
		pa_threaded_mainloop_free(m_ml);
		return;
	}
	pa_context_set_state_callback(m_ctx, context_state_cb, this);

	if (pa_context_connect(m_ctx, 0, (pa_context_flags_t)0, 0) < 0) {
		qCritical("PaClient: failed to connect");
		pa_context_unref(m_ctx);
		unlock();
		pa_threaded_mainloop_free(m_ml);
		return;
	}

	if (pa_threaded_mainloop_start(m_ml) != 0) {
		qCritical("PaClient: failed to start mainloop");
		pa_context_unref(m_ctx);
		unlock();
		pa_threaded_mainloop_free(m_ml);
		return;
	}

	unlock();

	m_init = true;
}
