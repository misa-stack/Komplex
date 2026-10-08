# Komplex

A C++ / Qt 6 desktop fractal explorer with offline English and Czech lessons.
Explore Mandelbrot, Julia, Burning Ship, and Tricorn sets, with beginner explanations,
mathematics, history, source references, and guided views for each.

## Build and run

Requires a C++17 compiler, CMake 3.21+, and Qt 6.2+ development packages with
Widgets and Test. The application uses CPU rendering; no GPU compute support,
SDL, network connection, or external font assets are required.

```sh
make
./program
```

Or use CMake directly:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 4
./build/program
```

The executable works from any directory. `make` copies it to `./program` for
compatibility with the original project. `make debug` uses `build-debug` and also
copies the debug executable to `./program`; a subsequent `make` restores release.
In Qt Creator, open `CMakeLists.txt` for automatic Qt include paths and build targets.
The existing generic project's file list is also updated to include the new sources.
The old SDL helper sources and original `komplex` binary are retained for reference
and are excluded from the new build. Run **program**, not the old **komplex** binary.

## Controls

- Select a fractal in the sidebar. Its current view is remembered for this session.
- Drag the image to pan. Scroll to zoom around the pointer.
- Click the image for keyboard control: hold arrows to pan, P to zoom in, O to zoom out.
- Use + / − for centered zoom. Reset restores that fractal's original view and parameters.
- Increase the iteration limit for more boundary detail. Julia also has editable real
  and imaginary components and three parameter presets.
- The learning panel has Beginner, Mathematics, and History tabs. “Try this view”
  loads the exploration described beneath the lesson.
- Hide the learning panel for a larger canvas or drag the divider to resize it.
- Switch English / Čeština at any time. Only the language persists between launches.
  Settings use Qt's standard per-user location under organization/application `Komplex`.

Burning Ship uses a downward-positive imaginary axis to display the conventional
upright ship. Other fractals use an upward-positive imaginary axis. All viewports
preserve the complex plane's aspect ratio. Panning is bounded to centers in [-10, 10]
on each axis; maximum visible width is 12 complex units. Zoom stops before neighboring
pixels become numerically indistinguishable in double precision. Arbitrary-precision
deep zoom, image export, and GPU computation are not included.

## Rendering

The UI thread paints the last image transformed to the current view immediately.
A persistent pool of 1–12 workers, based on available CPU threads, calculates tiled
images separately. Input invalidates obsolete jobs. Reduced-resolution previews use
up to 180 iterations; after 180 ms without changes, a full-resolution image uses the
selected 100–5000 iterations. Finished images are delivered on the UI thread. A stable
view does not keep recalculating.

Continuous escape-time colors reduce iteration banding. Dark pixels mean “not escaped
within this limit,” except for Mandelbrot's analytic interior checks; finite iteration
is not a general proof of membership. Julia displays the filled set and its boundary.

## Checks and diagnostics

```sh
make test
# Or: ctest --test-dir build --output-on-failure
QT_QPA_PLATFORM=offscreen ./build/program --benchmark
QT_QPA_PLATFORM=offscreen ./build/program --screenshot /tmp/komplex.png
./program --language cs
```

Tests isolate their settings in a temporary directory. They cover mathematical
recurrences, bounded/escaping samples, independent reference computations, coordinate
mapping, zoom anchoring, uneven tile dimensions, cancellation, shutdown, UI controls,
language persistence, resizing, idle rendering, and navigation-to-paint latency.
The UI tests also save screenshots in `build/qa` for visual review. Offscreen checks
exercise widgets and input dispatch; display-server behavior and actual monitor frame
pacing can differ.

Lesson text is compiled into the executable in `lessons.cpp`, with the corresponding
published references linked in each lesson. Only opening those links needs internet.
Measured rendering times and completed checks are recorded in `VERIFICATION.md`.
