// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/canvas.hpp"
#include "libfenster/platform/platform.hpp"

namespace fenster
{
	Canvas* Canvas::create()
	{
		return createCanvasComponent<Canvas>();
	}

	void Canvas::acknowledgeNewBuffer(uintptr_t address, uint16_t width, uint16_t height)
	{
		bool changed = false;

		platformAcquireMutex(currentBufferLock);
		if(address != (uintptr_t) currentBuffer.buffer)
		{
			if(currentBuffer.surface)
			{
				cairo_surface_destroy(currentBuffer.surface);
				currentBuffer.surface = nullptr;
			}
			if(currentBuffer.buffer)
			{
				platformUnmapSharedMemory(currentBuffer.buffer);
			}

			currentBuffer.buffer = (uint8_t*) address;
			currentBuffer.width = width;
			currentBuffer.height = height;

			if(currentBuffer.buffer)
			{
				currentBuffer.surface = cairo_image_surface_create_for_data(
						currentBuffer.buffer, CAIRO_FORMAT_ARGB32,
						currentBuffer.width, currentBuffer.height,
						cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, currentBuffer.width)
						);

				auto surfaceStatus = cairo_surface_status(currentBuffer.surface);
				if(surfaceStatus != CAIRO_STATUS_SUCCESS)
				{
					platformLog("failed to create surface for canvas %i: %i", this->id, surfaceStatus);
					currentBuffer.surface = nullptr;
				}
			}

			changed = true;
		}
		platformReleaseMutex(currentBufferLock);

		if(changed && bufferListener)
			bufferListener->handleBufferChanged();
	}

	cairo_t* Canvas::acquireGraphics()
	{
		platformAcquireMutex(currentBufferLock);
		if(!currentBuffer.surface)
		{
			platformReleaseMutex(currentBufferLock);
			return nullptr;
		}

		if(!currentBuffer.context)
		{
			currentBuffer.context = cairo_create(currentBuffer.surface);
			auto contextStatus = cairo_status(currentBuffer.context);
			if(contextStatus != CAIRO_STATUS_SUCCESS)
			{
				platformLog("failed to create cairo context: %i", contextStatus);
				currentBuffer.context = nullptr;
				platformReleaseMutex(currentBufferLock);
				return nullptr;
			}
		}

		currentBuffer.contextRefCount++;
		return currentBuffer.context;
	}

	void Canvas::releaseGraphics()
	{
		--currentBuffer.contextRefCount;
		if(currentBuffer.contextRefCount < 0)
		{
			platformLog("error: over-deref of Canvas %i by %i references", this->id, currentBuffer.contextRefCount);
			currentBuffer.contextRefCount = 0;
			return;
		}

		if(currentBuffer.contextRefCount == 0 && currentBuffer.context)
		{
			cairo_surface_flush(currentBuffer.surface);
			cairo_destroy(currentBuffer.context);
			currentBuffer.context = nullptr;
		}

		platformReleaseMutex(currentBufferLock);
	}

	void Canvas::blit(const Rectangle& rect)
	{
		SYS_TX_T tx = platformCreateMessageTransaction();
		CommandCanvasBlitRequest request;
		request.header.id = fenster::ProtocolCommandId::CanvasBlit;
		request.id = this->id;
		request.area = rect;
		platformSendMessage(DelegateTaskId, &request, sizeof(CommandCanvasBlitRequest), tx);
		platformYieldTo(DelegateTaskId);
	}
}
