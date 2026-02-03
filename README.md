# 🌀 COMPLEX // FRACTAL_EXPLORER
> **Project Type:** Real-time Mathematical Visualizer  
> **Core Tech:** C++, SIMD (AVX2), Multi-threading

### 📂 SYSTEM_OVERVIEW
"Complex" is an infinite-zoom engine for Mandelbrot and Julia sets. The goal was to solve the computational bottleneck of escape-time algorithms using modern CPU instruction sets.

### 🛠 TECHNICAL_SOLUTIONS
- **SIMD Parallelization:** Utilizes **AVX2 instructions** to calculate four complex number iterations simultaneously per core.
- **Multi-threading:** Dynamically partitions the screen into "tiles," distributing the workload across all available logical processors.
- **Precision:** Implemented custom fixed-point math to maintain zoom stability beyond standard `double` precision limits.

### 🚀 USAGE
- **Arrows:** Navigate Complex Plane
- **Scroll:** Infinite Zoom
- **Space:** Toggle Julia/Mandelbrot Mode

- ## 🛠 Installation & Setup

### Prerequisites
This project requires a C++ compiler (GCC/Clang) and the **SDL2** development library.

#### 🐧 Arch Linux
sudo pacman -S base-devel sdl2

git clone https://github.com/misa-stack/Complex.git
cd Complex
g++ Complex.cpp -o complex -lSDL2
