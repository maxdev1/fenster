// fenster — lightweight window server and UI toolkit
// Ported to SDL2 + cairo for macOS (Apple platforms)
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef __APPLE__

#include "macos_sdl_sim.hpp"

#include <server.hpp>
#include <components/cursor.hpp>
#include <libfenster/input/key_info.hpp>

#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <dirent.h>
#include <string>
#include <cstring>
#include <cstdint>

// keep previous constants
static const int WIDTH = 1600;
static const int HEIGHT = 1200;

// pixel buffer for cairo; using uint32_t per-pixel ARGB32
static unsigned int pixels[WIDTH * HEIGHT];

namespace fensterserver
{
    Server* server = nullptr;
}

// --- small helper to map SDL keys to your engine's string keys ---
// extend as needed


// --- SDL / cairo main loop + video output implementation --------------------

int main(int argc, char** argv)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // create window centered
    SDL_Window* win = SDL_CreateWindow("fenster",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WIDTH, HEIGHT,
                                       SDL_WINDOW_SHOWN /* | other flags if you want */);

    if (!win)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // renderer + streaming texture
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Streaming texture that will hold our pixel buffer presented by cairo
    SDL_Texture* tex = SDL_CreateTexture(ren,
                                         SDL_PIXELFORMAT_ARGB8888,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         WIDTH, HEIGHT);
    if (!tex)
    {
        SDL_Log("SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Create cairo surface that points at 'pixels' buffer:
    // Using CAIRO_FORMAT_ARGB32 -> 32-bit with A in high byte for cairo's native layout
    cairo_surface_t* surface = cairo_image_surface_create_for_data(
        (unsigned char*)pixels,
        CAIRO_FORMAT_ARGB32,
        WIDTH, HEIGHT,
        WIDTH * 4);

    cairo_t* cr = cairo_create(surface);

    // create server and start platform server thread (same as original)
    fensterserver::server = new fensterserver::Server();
    fenster::platformCreateThread((void*) &fensterserver::platformServerThread);

    // state like original
    bool running = true;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;

    // ensure cursor visible initially
    SDL_ShowCursor(SDL_ENABLE);

    // main loop
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                {
                    // map key
                    std::string key = fensterserver::platformKeyForSDLKey(e.key.keysym);

                    // update modifier state explicitly for left/right keys too
                    if (key == "KEY_SHIFT_L" || key == "KEY_SHIFT_R") shift = true;
                    if (key == "KEY_CTRL_L" || key == "KEY_CTRL_R")   ctrl = true;
                    if (key == "KEY_ALT_L" || key == "KEY_ALT_R")     alt = true;

                    // ctrl+Q toggles debug like original (we only handle left ctrl state too)
                    if ( (ctrl || (e.key.keysym.mod & KMOD_CTRL)) && key == "KEY_Q" )
                    {
                        fensterserver::Server::setDebug(!fensterserver::Server::isDebug());
                        break;
                    }

                    // escape to quit (like original)
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                        break;
                    }

                    fenster::KeyInfo info;
                    info.pressed = true;
                    info.key = key;
                    info.ctrl = ctrl || (e.key.keysym.mod & KMOD_CTRL);
                    info.shift = shift || (e.key.keysym.mod & KMOD_SHIFT);
                    info.alt = alt || (e.key.keysym.mod & KMOD_ALT);
                    info.pressed = true;
                    fensterserver::Server::instance()->eventProcessor->bufferKeyEvent(info);
                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }

                case SDL_KEYUP:
                {
                    std::string key = fensterserver::platformKeyForSDLKey(e.key.keysym);

                    if (key == "KEY_SHIFT_L" || key == "KEY_SHIFT_R") shift = false;
                    if (key == "KEY_CTRL_L" || key == "KEY_CTRL_R")   ctrl = false;
                    if (key == "KEY_ALT_L" || key == "KEY_ALT_R")     alt = false;

                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        running = false;
                        break;
                    }

                    fenster::KeyInfo info;
                    info.scancode = e.key.keysym.scancode;
                    info.key = key;
                    info.ctrl = ctrl || (e.key.keysym.mod & KMOD_CTRL);
                    info.shift = shift || (e.key.keysym.mod & KMOD_SHIFT);
                    info.alt = alt || (e.key.keysym.mod & KMOD_ALT);
                    info.pressed = false;
                    fensterserver::Server::instance()->eventProcessor->bufferKeyEvent(info);
                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }

                case SDL_MOUSEMOTION:
                {
                    int mouseX = e.motion.x;
                    int mouseY = e.motion.y;

                    // replicate ShowCursor logic: hide cursor when y >= 10, show when y < 10
                    if (mouseY < 10)
                    {
                        SDL_ShowCursor(SDL_ENABLE);
                    }
                    else
                    {
                        SDL_ShowCursor(SDL_DISABLE);
                    }

                    fensterserver::Cursor::nextPosition.x = mouseX;
                    fensterserver::Cursor::nextPosition.y = mouseY;

                    if (fensterserver::Cursor::nextPosition.x < 0) fensterserver::Cursor::nextPosition.x = 0;
                    if (fensterserver::Cursor::nextPosition.x > WIDTH - 2) fensterserver::Cursor::nextPosition.x = WIDTH - 2;
                    if (fensterserver::Cursor::nextPosition.y < 0) fensterserver::Cursor::nextPosition.y = 0;
                    if (fensterserver::Cursor::nextPosition.y > HEIGHT - 2) fensterserver::Cursor::nextPosition.y = HEIGHT - 2;

                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    if (e.button.button == SDL_BUTTON_LEFT)
                        fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button1;
                    if (e.button.button == SDL_BUTTON_RIGHT)
                        fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button2;
                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }

                case SDL_MOUSEBUTTONUP:
                {
                    if (e.button.button == SDL_BUTTON_LEFT)
                        fensterserver::Cursor::nextPressedButtons &= ~fenster::MouseButton::Button1;
                    if (e.button.button == SDL_BUTTON_RIGHT)
                        fensterserver::Cursor::nextPressedButtons &= ~fenster::MouseButton::Button2;
                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }

                case SDL_MOUSEWHEEL:
                {
                    // SDL mouse wheel gives steps in .y (positive away from the user)
                    // original code did: steps = delta / WHEEL_DELTA; nextScroll += -steps;
                    // We'll do similar: mouse wheel y = steps
                    int steps = e.wheel.y;
                    fensterserver::Cursor::nextScroll += -steps;
                    fensterserver::Server::instance()->requestUpdateImmediately();
                    break;
                }
            } // switch event.type
        } // while poll event

        // present the pixel buffer produced by cairo
        // update SDL texture from our pixels buffer
        // note: assume pixels are in ARGB8888 which matches SDL_PIXELFORMAT_ARGB8888
        SDL_UpdateTexture(tex, nullptr, pixels, WIDTH * 4);

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, nullptr, nullptr);
        SDL_RenderPresent(ren);

        // throttle roughly to ~60fps like original Sleep(16)
        SDL_Delay(16);
    } // main loop

    // cleanup
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
} // main


// ---------------- platform glue (same semantics as original) -----------------

namespace fensterserver
{
    class sdl_video_output_t : public VideoOutput
    {
    public:
        explicit sdl_video_output_t() = default;

        bool initialize() override
        {
            // nothing to do here; pixels buffer already allocated
            return true;
        }

        // blit copies a rectangle from 'source' into our pixels buffer
        void blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source) override
        {
            for(int y = 0; y < invalid.height; ++y)
            {
                fenster::ColorArgb* srcRow = source + (invalid.y + y) * sourceSize.width + invalid.x;
                fenster::ColorArgb* dstRow = (fenster::ColorArgb*) (pixels + (invalid.y + y) * WIDTH + invalid.x);
                // copy bytes for the row
                std::memcpy(dstRow, srcRow, invalid.width * sizeof(fenster::ColorArgb));
            }
        }

        fenster::Dimension getResolution() override
        {
            return fenster::Dimension(WIDTH, HEIGHT);
        }
    };

    // return a new VideoOutput (SDL-backed)
    VideoOutput* platformCreateVideoOutput()
    {
        return new sdl_video_output_t();
    }

    void platformServerThread()
    {
        server->launch();
    }

    void platformStartInput()
    {
        // SDL event loop is handled on main thread here; nothing to do
    }

    bool platformInitializeKeyboardLayout(std::string layout)
    {
        // no-op for now
        (void)layout;
        return true;
    }

	void platformLoadCursors()
	{
		const char* path = "platform/sim/cursor";
		DIR* dir = opendir(path);
		if(!dir)
		{
			return;
		}
		struct dirent* entry;
		while((entry = readdir(dir)) != NULL)
		{
			std::string path = std::string("platform/sim/cursor") + "/" + entry->d_name;
			Cursor::load(path);
		}
		Cursor::set("default");
		closedir(dir);
	}

}

#endif
