# macOS component testing

Dependencies: Homebrew GCC 15, SDL2, Cairo and FreeType.

From the Ghost repository root:

```sh
make -C applications/fenster target=macos all
(cd applications/fenster && FENSTER_DEMO=1 ./fenster)
python3 applications/fenster/server/tests/run_controls.py
```

`FENSTER_DEMO=1` opens an explorer-style gallery of the actual server components.
Without it, the original component demos open. The simulator does not implement
client IPC, so the gallery is a component preview; test Navigator itself in Ghost.

For a quick render without a desktop window:

```sh
(cd applications/fenster && SDL_VIDEODRIVER=dummy FENSTER_DEMO=1 \
  FENSTER_CAPTURE=/tmp/fenster-controls.png ./fenster)
```

The capture exits after 90 frames. Libraries live under `obj/<target>/`, so
switching between macOS and Ghost does not mix native and cross-compiled archives.
After header/layout changes, clean affected targets before rebuilding.
