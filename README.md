<h1>Particle Simulator</h1>

A C++ particle simulation built with raylib, focused on physics simulation, spatial optimization and multithreading.

## Features

* Particle physics simulation
* Gravity and wall collisions
* Particle-to-particle collision detection
* Spatial Grid for efficient collision detection
* Basic multithreading for parallel particle updates
* Real-time FPS counter
* Support for large particle counts

## Performance

The simulation is designed to efficiently handle large numbers of particles.

With the Spatial Grid, particles only check nearby grid cells instead of comparing every particle with every other particle.

### Time Complexity

Without spatial partitioning:

```text
O(n²)
```

With the Spatial Grid, assuming particles are reasonably distributed:

```text
O(n)
```

The collision search checks the particle's own cell and its 8 neighboring cells:

```text
3 × 3 = 9 cells
```

So more explicitly:

```text
O(n × 9 × k)
```

where `k` is the average number of particles in the checked cells.

Since `9` and, under normal conditions, `k` are constants, this simplifies to approximately:

```text
O(n)
```

## Current Goal

The project is being optimized to handle increasingly large particle counts while maintaining a high and stable FPS.

Current benchmark:

```text
25.000 particles → ~60–80 FPS
```

## Technologies

* raylib
* Spatial partitioning
* Multithreading
