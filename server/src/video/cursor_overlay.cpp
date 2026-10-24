// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "video/cursor_overlay.hpp"
#include "components/cursor.hpp"
#include <algorithm>
#include <cstring>

namespace fensterserver
{
    CursorOverlay::CursorOverlay(VideoOutput* output, const fenster::Rectangle& bounds) :
        output(output), bounds(bounds)
    {
        presentation.resize(bounds.width, bounds.height, false);
        auto cr = presentation.acquireContext();
        if(cr)
        {
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_paint(cr);
            presentation.releaseContext();
        }
    }

    void CursorOverlay::restoreCursor()
    {
        auto surface = presentation.getSurface();
        cairo_surface_flush(surface);
        auto pixels = cairo_image_surface_get_data(surface);
        const auto stride = cairo_image_surface_get_stride(surface);
        for(int y = 0; y < cursorArea.height; y++)
        {
            memcpy(pixels + (cursorArea.y + y) * stride + cursorArea.x * sizeof(fenster::ColorArgb),
                   &savedPixels[y * cursorArea.width], cursorArea.width * sizeof(fenster::ColorArgb));
        }
        cairo_surface_mark_dirty_rectangle(surface, cursorArea.x, cursorArea.y, cursorArea.width, cursorArea.height);
    }

    void CursorOverlay::saveAndPaintCursor()
    {
        fenster::platformAcquireMutex(Cursor::inputLock);
        const auto position = Cursor::nextPosition;
        fenster::platformReleaseMutex(Cursor::inputLock);

        // Retain one immutable image without holding its configuration lock while painting.
        auto image = Cursor::snapshot();
        cursorArea = image.getAreaAt(position).clip(bounds);
        savedPixels.resize(cursorArea.width * cursorArea.height);
        auto surface = presentation.getSurface();
        cairo_surface_flush(surface);
        const auto pixels = cairo_image_surface_get_data(surface);
        const auto stride = cairo_image_surface_get_stride(surface);
        for(int y = 0; y < cursorArea.height; y++)
        {
            memcpy(&savedPixels[y * cursorArea.width],
                   pixels + (cursorArea.y + y) * stride + cursorArea.x * sizeof(fenster::ColorArgb),
                   cursorArea.width * sizeof(fenster::ColorArgb));
        }
        image.paint(surface, position, cursorArea);
    }

    void CursorOverlay::flush(const fenster::Rectangle& area)
    {
        if(area.width <= 0 || area.height <= 0)
            return;

        auto surface = presentation.getSurface();
        cairo_surface_flush(surface);
        auto pixels = reinterpret_cast<fenster::ColorArgb*>(cairo_image_surface_get_data(surface));
        output->blit(area, bounds, pixels);
    }

    void CursorOverlay::flushChanges(const fenster::Rectangle& damage, const fenster::Rectangle& oldCursor)
    {
        flush(damage);
        if(oldCursor.clip(damage) != oldCursor)
            flush(oldCursor);
        if(cursorArea.clip(damage) != cursorArea && cursorArea != oldCursor)
            flush(cursorArea);
    }

    void CursorOverlay::present()
    {
        fenster::platformAcquireMutex(lock);
        const auto oldCursor = cursorArea;
        restoreCursor();
        saveAndPaintCursor();
        flushChanges(fenster::Rectangle(), oldCursor);
        fenster::platformReleaseMutex(lock);
    }

    void CursorOverlay::commit(Graphics* scene, const fenster::Rectangle& requestedDamage)
    {
        const auto damage = requestedDamage.clip(bounds);
        if(damage.width <= 0 || damage.height <= 0)
            return;

        auto sourceSurface = scene->getSurface();
        auto targetSurface = presentation.getSurface();
        cairo_surface_flush(sourceSurface);
        const auto source = cairo_image_surface_get_data(sourceSurface);
        const auto sourceStride = cairo_image_surface_get_stride(sourceSurface);
        auto target = cairo_image_surface_get_data(targetSurface);
        const auto targetStride = cairo_image_surface_get_stride(targetSurface);
        constexpr int maxBandBytes = 128 * 1024;
        const int rowsPerBand = std::max(1, maxBandBytes / (damage.width * int(sizeof(fenster::ColorArgb))));
        for(int top = damage.y; top < damage.getBottom(); top += rowsPerBand)
        {
            const fenster::Rectangle band(damage.x, top, damage.width,
                                           std::min(rowsPerBand, damage.getBottom() - top));
            // Bound presentation ownership; each band also presents the latest pointer.
            // Component traversal and scene rendering have already finished.
            fenster::platformAcquireMutex(lock);
            const auto oldCursor = cursorArea;
            restoreCursor();
            cairo_surface_flush(targetSurface);
            for(int y = band.y; y < band.getBottom(); y++)
            {
                memcpy(target + y * targetStride + band.x * sizeof(fenster::ColorArgb),
                       source + y * sourceStride + band.x * sizeof(fenster::ColorArgb),
                       band.width * sizeof(fenster::ColorArgb));
            }
            cairo_surface_mark_dirty_rectangle(targetSurface, band.x, band.y, band.width, band.height);

            saveAndPaintCursor();
            flushChanges(band, oldCursor);
            fenster::platformReleaseMutex(lock);
        }
    }
}
