// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _GHOST_

#include "platform/platform.hpp"
#include "generic_video_output.hpp"

g_video_output* platformCreateVideoOutput()
{
#ifdef _GHOST_
	g_set_video_log(false);
	platformLog("waiting for a video device to be present...");

	auto tx = G_MESSAGE_TOPIC_TRANSACTION_START;
	size_t bufLen = 1024;
	uint8_t buf[bufLen];
	while(true)
	{
		auto status = g_receive_topic_message(G_DEVICE_EVENT_TOPIC, buf, bufLen, tx);
		if(status == G_MESSAGE_RECEIVE_STATUS_SUCCESSFUL)
		{
			auto message = (g_message_header*) buf;
			tx = message->transaction;
			auto content = (g_device_event_header*) G_MESSAGE_CONTENT(message);

			if(content->event == G_DEVICE_EVENT_DEVICE_REGISTERED)
			{
				auto deviceEvent = (g_device_event_device_registered*) content;

				if(deviceEvent->type == G_DEVICE_TYPE_VIDEO)
				{
					return new g_generic_video_output(deviceEvent->driver, deviceEvent->id);
				}
			}
		}
	}
#endif
}

#endif