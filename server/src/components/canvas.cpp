// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/canvas.hpp"
#include "process_registry.hpp"
#include "server.hpp"

#include <cstring>

#define ALIGN_UP(value) (value + value % 100)

namespace fensterserver
{
	Canvas::Canvas(SYS_TID_T partnerThread)
	{
		partnerProcess = fenster::platformGetPidForTid(partnerThread);
		asyncInfo = new async_resizer_info_t();
		asyncInfo->canvas = this;
		asyncInfo->alive = true;
		asyncInfo->lock = fenster::platformInitializeMutex(false);
		asyncInfo->checkAtom = fenster::platformInitializeMutex(false);
		fenster::platformCreateThreadWithData((void*) Canvas::asyncBufferResizer, asyncInfo);
	}

	/**
	 * The asyncBufferResizer runs a method on our canvas, so we must use this lock
	 * to avoid destroying the canvas while it is executed.
	 */
	Canvas::~Canvas()
	{
		fenster::platformAcquireMutex(asyncInfo->lock);
		asyncInfo->alive = false;
		fenster::platformReleaseMutex(asyncInfo->lock);
	}

	void Canvas::asyncBufferResizer(async_resizer_info_t* asyncInfo)
	{
		while(true)
		{
			fenster::platformAcquireMutexTimeout(asyncInfo->checkAtom, 10000);

			fenster::platformAcquireMutex(asyncInfo->lock);
			if(!asyncInfo->alive)
				break;
			asyncInfo->canvas->checkBuffer();
			fenster::platformReleaseMutex(asyncInfo->lock);
		}
		delete asyncInfo;
	}

	/**
	 * When the bounds of a canvas are changed, the buffer must be checked.
	 */
	void Canvas::handleBoundChanged(const fenster::Rectangle& oldBounds)
	{
		fenster::platformReleaseMutex(asyncInfo->checkAtom);
	}

	/**
	 * Checks whether the current buffer is still sufficient for the required amount of pixels.
	 */
	void Canvas::checkBuffer()
	{
		fenster::platformAcquireMutex(lock);
		fenster::Rectangle bounds = getBounds();
		if(bounds.width == 0 || bounds.height == 0)
		{
			fenster::platformReleaseMutex(lock);
			return;
		}

		bounds.width = ALIGN_UP(bounds.width);
		bounds.height = ALIGN_UP(bounds.height);

		createNewBuffer(bounds, bounds.width, bounds.height);
		fenster::platformReleaseMutex(lock);
	}

	void Canvas::createNewBuffer(fenster::Rectangle& bounds, int width, int height)
	{
		// Check if buffer still large enough
		int stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, width);
		int imageSize = stride * height;
		uint16_t pages = SYS_PAGE_ALIGN_UP(imageSize) / SYS_PAGE_SIZE;
		uint32_t bufferSize = pages * SYS_PAGE_SIZE;

		fenster::platformAcquireMutex(bufferLock);
		if(pages <= buffer.pages)
		{
			fenster::platformReleaseMutex(bufferLock);
			return;
		}

		// Destroy old resources
		if(buffer.surface)
		{
			cairo_surface_destroy(buffer.surface);
			buffer.surface = nullptr;
		}
		if(buffer.localMapping)
		{
			fenster::platformUnmapSharedMemory(buffer.localMapping);
			buffer.localMapping = nullptr;
		}

		// create a new buffer
		buffer.pages = pages;
		buffer.localMapping = (uint8_t*) fenster::platformAllocateMemory(bufferSize);
		buffer.paintableWidth = bounds.width;
		buffer.paintableHeight = bounds.height;

		if(buffer.localMapping == nullptr)
		{
			fenster::platformLog("warning: failed to allocate a buffer of size %i for a canvas", bufferSize);
		}
		else
		{
			memset(buffer.localMapping, 0, bufferSize);
			buffer.remoteMapping = (uint8_t*) fenster::platformShareMemory(buffer.localMapping, pages * SYS_PAGE_SIZE,
			                                                               partnerProcess);
			if(buffer.remoteMapping == nullptr)
			{
				fenster::platformLog("warning: failed to share a buffer for a canvas to proc %i", partnerProcess);
			}
			else
			{
				buffer.surface = cairo_image_surface_create_for_data(
						buffer.localMapping, CAIRO_FORMAT_ARGB32, buffer.paintableWidth, buffer.paintableHeight,
						cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, buffer.paintableWidth));

				auto status = cairo_surface_status(buffer.surface);
				if(status != CAIRO_STATUS_SUCCESS)
				{
					fenster::platformLog("failed to create surface: %i", status);
					buffer.surface = nullptr;
				}
			}
		}

		notifyClientAboutNewBuffer();
		fenster::platformReleaseMutex(bufferLock);
	}

	void Canvas::notifyClientAboutNewBuffer()
	{
		fenster::ComponentCanvasWaitForAcknowledgeEvent event;
		event.header.type = fenster::ComponentEventType::CanvasNewBuffer;
		event.header.component_id = this->id;
		event.newBufferAddress = (uintptr_t) buffer.remoteMapping;
		event.width = buffer.paintableWidth;
		event.height = buffer.paintableHeight;

		SYS_TID_T eventDispatcher = ProcessRegistry::get(partnerProcess);
		if(eventDispatcher == SYS_TID_NONE)
		{
			fenster::platformLog(
					"failed to send buffer notification to event dispatcher of process %i since it is not registered",
					partnerProcess);
		}
		else
		{
			platformSendMessage(eventDispatcher, &event, sizeof(fenster::ComponentCanvasWaitForAcknowledgeEvent),
			                    SYS_TX_NONE);
		}
	}

	void Canvas::blit(Graphics* out, const fenster::Rectangle& clip, const fenster::Point& positionOnParent)
	{
		auto cr = out->acquireContext();
		if(cr)
		{
			fenster::platformAcquireMutex(bufferLock);
			if(bufferReady && buffer.surface)
			{
				cairo_surface_mark_dirty(buffer.surface);
				cairo_save(cr);
				cairo_set_source_surface(cr, buffer.surface, positionOnParent.x, positionOnParent.y);
				cairo_paint(cr);
				cairo_restore(cr);
			}
			fenster::platformReleaseMutex(bufferLock);

			if(Server::isDebug())
			{
				cairo_save(cr);
				cairo_set_line_width(cr, 2);
				cairo_rectangle(cr, clip.x, clip.y, clip.width, clip.height);
				cairo_set_source_rgba(cr, 1, 0, 0, 1);
				cairo_stroke(cr);
				cairo_restore(cr);
			}

			out->releaseContext();
		}

		this->blitChildren(out, clip, positionOnParent);
	}


	void Canvas::requestBlit(fenster::Rectangle& area)
	{
		fenster::platformAcquireMutex(bufferLock);
		bufferReady = true;
		fenster::platformReleaseMutex(bufferLock);

		markDirty(area);
		Server::instance()->requestUpdateLater();
	}
}
