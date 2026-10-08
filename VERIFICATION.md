# Verification

Verified on this Linux workspace with GCC 16.2.1 and Qt 6.11.2, Release build.

- Direct CMake configure/build: passed.
- `make test`: passed (7 test cases, plus initialization and cleanup).
- Native desktop launch, render, screenshot, and clean exit: passed.
- English/Czech UI screenshots, small-window layouts, and 2× display scaling:
  inspected. Sidebar scrolls when the Julia controls exceed the window height.
- Navigation event to paint: 4 ms in the offscreen widget test. This is input/paint
  latency, not a claim about monitor refresh rate or operating-system frame pacing.
- Stable-view test confirms no additional rendering jobs once refinement completes.

Final `./program --benchmark` measurements, 600 iterations and 7 worker threads:

| Fractal | 800 × 600 | 1600 × 900 |
|---|---:|---:|
| Mandelbrot | 26 ms | 72 ms |
| Julia | 43 ms | 125 ms |
| Burning Ship | 78 ms | 252 ms |
| Tricorn | 44 ms | 141 ms |

These are full-detail default views. Timings vary with CPU load, parameter choices,
zoom position, resolution, and iteration count. Interactive previews use fewer pixels
and iterations. Screenshot artifacts are generated under `build/qa` and are not
required at runtime.
