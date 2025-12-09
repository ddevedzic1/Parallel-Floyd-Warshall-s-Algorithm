# Parallel All-Pairs Shortest Path

15 verzija mplementacije Floyd-Warshall-ovog algoritma sa različitim optimizacijama: loop unrolling, klasični blocking, generalni bloking, SIMD (AVX2) i OpenMP paralelizacija i mnoge druge.

## Sistemski zahtjevi

- C++17 kompajler (GCC 9+, Clang 10+, ili MSVC 2019+)
- CMake 3.16+
- Podrška za AVX2 instrukcije
- OpenMP biblioteka

## Build instrukcije

### Linux / macOS
```bash
cd PAPSP
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Windows (Visual Studio)
```bash
cd PAPSP
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Windows (MinGW)
```bash
cd PAPSP
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

Nakon uspješnog builda, izvršne datoteke se nalaze u:
- **Linux/macOS:** `build/`
- **Windows:** `build/Release/`

## Dostupni algoritmi

### Baseline verzija
| Naziv | Opis |
|-------|------|
| `baseline` | Osnovni Floyd-Warshall bez optimizacija |
| `baseline_unroll` | Sa loop unrolling i malim dodatnim optimizacijama |
| `baseline_simd` | Sa SIMD (AVX2) optimizacijom |
| `baseline_parallel` | Sa OpenMP paralelizacijom |
| `baseline_parallel_simd` | Sa OpenMP + SIMD |

### Blocking verzije
| Naziv | Opis |
|-------|------|
| `blocking_baseline` | Blocked Floyd-Warshall bez dodatnih optimizacija |
| `blocking_unroll` | Sa loop unrolling (8×8) |
| `blocking_simd` | Sa SIMD (AVX2) |
| `blocking_parallel` | Sa OpenMP paralelizacijom |
| `blocking_parallel_simd` | Sa OpenMP + SIMD |

### Blocking-Generalized verzije (multi-layer)
| Naziv | Opis |
|-------|------|
| `blocking_generalized_baseline` | Generalizirani blocked algoritam |
| `blocking_generalized_unroll` | Sa loop unrolling i malim dodatnim optimizacijama |
| `blocking_generalized_simd` | Sa SIMD (AVX2) |
| `blocking_generalized_parallel` | Sa OpenMP paralelizacijom |
| `blocking_generalized_parallel_simd` | Sa OpenMP + SIMD |

## Pokretanje

### Quick Runner (Preporučeno za testiranje)

Najjednostavniji način za testiranje performansi algoritama. Generiše graf, izvršava algoritam i mjeri vrijeme.
```
Sintaksa: quick_runner <algorithm_name> <n_size> <density> [seed] [block_size]

Parametri:
  algorithm_name  - Naziv algoritma (vidjeti listu iznad)
  n_size          - Broj čvorova grafa
  density         - Gustina grafa (0.0 - 1.0)
  seed            - (Opcionalno) Seed za generator slučajnih brojeva
  block_size      - (Opcionalno) Veličina bloka za blocking verzije
```

**Primjeri:**
```bash
# Linux
./quick_runner blocking_parallel_simd 4096 0.9 42 64
./quick_runner baseline 1024 0.8

# Windows (iz build/Release foldera)
quick_runner blocking_parallel_simd 4096 0.9 42 64
quick_runner baseline 1024 0.8
```

### Tests Runner (Testiranje ispravnosti)

Pokreće skup testova za verifikaciju ispravnosti algoritma.
```
Sintaksa: tests_runner <algorithm_name> [block_size]

Parametri:
  algorithm_name  - Naziv algoritma
  block_size      - (Opcionalno) Veličina bloka, default: 64
```

**Primjeri:**
```bash
# Linux
./tests_runner blocking_parallel 64
./tests_runner baseline_unroll

# Windows
tests_runner blocking_parallel 64
tests_runner baseline_unroll
```

### Graph Generator CLI (Generisanje grafova)

Generiše graf i sprema ga u binarnu datoteku za kasniji benchmarking.
```
Sintaksa: graph_generator_cli <n_size> <density> [seed]

Parametri:
  n_size   - Broj čvorova grafa
  density  - Gustina grafa (0.0 - 1.0)
  seed     - (Opcionalno) Seed za generator slučajnih brojeva
```

**Primjer:**
```bash
./graph_generator_cli 2048 0.9 42
```

Generisani graf se sprema u `data/` folder.

### Bench Runner (Precizniji benchmarking)

Učitava prethodno generisan graf iz datoteke i izvršava algoritam. Koristi se za precizniji benchmarking bez overhead-a generisanja grafa.
```
Sintaksa: bench_runner <algorithm_name> <graph_file_path> [block_size]

Parametri:
  algorithm_name   - Naziv algoritma
  graph_file_path  - Putanja do binarne datoteke grafa
  block_size       - (Opcionalno) Veličina bloka
```

**Primjer:**
```bash
# Prvo generisati graf
./graph_generator_cli 2048 0.9 42

# Zatim pokrenuti benchmark
./bench_runner blocking_parallel_simd data/graph_n2048_d90_s42.bin 64
```

## Napomene

- Za blocking verzije, preporučena veličina bloka je 64-128
- Gustina grafa 0.9 (90%) je dobra za testiranje performansi
