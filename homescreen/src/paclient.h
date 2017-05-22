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

#include <pulse/pulseaudio.h>

#include <QtCore/QHash>
#include <QtCore/QObject>

class PaClient : public QObject
{
	Q_OBJECT
	public:
		PaClient();
		~PaClient();

		void init();
		void close();

		inline pa_context *context() const
		{
			return m_ctx;
		}

		inline void lock()
		{
			pa_threaded_mainloop_lock(m_ml);
		}

		inline void unlock()
		{
			pa_threaded_mainloop_unlock(m_ml);
		}

		pa_sink_info * getDefaultSinkInfo(void);
		void setDefaultSinkInfo(const pa_sink_info *i);
		void setMasterVolume(const pa_cvolume *);

	public slots:
		void incDecVolume(const int volume_delta);

	signals:
		void volumeExternallyChanged(int volume);

	private:
		bool m_init;
		pa_threaded_mainloop *m_ml;
		pa_mainloop_api *m_mlapi;
		pa_context *m_ctx;
		pa_cvolume m_master_cvolume;
		pa_sink_info m_default_sink_info;
};
