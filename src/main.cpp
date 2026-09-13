#include <iostream> 
#include <vector> 
#include <stdlib.h> 
#include <algorithm>
#include <cmath> 
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <condition_variable>
#include "raylib.h" 
 
#include "Particle.hpp" 
 
#define screenWidth 1280 
#define screenHeight 720 

#define radius 1

constexpr int cellSize = 10;

constexpr int gridWidth = (screenWidth + cellSize - 1) / cellSize;
constexpr int gridHeight = (screenHeight + cellSize - 1) / cellSize;

int getCellIndex(Vector2 position)
{
   int cellX = position.x / cellSize;
   int cellY = position.y / cellSize;

   cellX = std::clamp(cellX, 0, gridWidth - 1);
   cellY = std::clamp(cellY, 0, gridHeight - 1);

   return cellY * gridWidth + cellX;
}

void updateParticle(std::vector<Particle>& particles, size_t start, size_t end) {
   for (size_t i = start; i < end; i++) {
      particles[i].move();
   }
}

int main() { 
   InitWindow(screenWidth, screenHeight, "Particle Simulator"); 
 
   std::vector<Particle> particles; 
   std::vector<std::vector<int>> grid(gridWidth * gridHeight);

   uint64_t count = 0; 

   while (count < 10000) { 
      count++; 
 
      Particle part{(float)GetRandomValue(0, screenWidth), (float)GetRandomValue(20, screenHeight),(float)GetRandomValue(-100, 100), 0 }; 
      particles.push_back(part); 
   } 
 
   std::mutex mutex;
   std::condition_variable startWork;
   std::condition_variable endWork;

   size_t middle = 0;
   std::atomic<bool> running = true;
   
   unsigned long frameNumber = 0;
   int finishedWorkers = 0;

   auto worker = [&](bool firstWorker) {
   unsigned long lastFrame = 0;

   while (true) {
      size_t start;
      size_t end;

      {
         std::unique_lock<std::mutex> lock(mutex);

         startWork.wait(lock, [&] {
              return frameNumber != lastFrame || !running;
         });

         if (!running) {
            return;
         }

         lastFrame = frameNumber;

         start = firstWorker ? 0 : middle;
         end = firstWorker ? middle : particles.size();
      }

      updateParticle(particles, start, end);

      {
         std::lock_guard<std::mutex> lock(mutex);

         finishedWorkers++;

         if (finishedWorkers == 2) {
            endWork.notify_one();
         }
      }
   }
};

   std::thread t1(worker, true);
   std::thread t2(worker, false);
   

   while(!WindowShouldClose()) { 
      // Drawing Particles and updating them
      {
         std::lock_guard<std::mutex> lock(mutex);

         middle = particles.size() / 2;
         finishedWorkers = 0;
         frameNumber++;
      }

      startWork.notify_all();

      {
         std::unique_lock<std::mutex> lock(mutex);

         endWork.wait(lock, [&]{
            return finishedWorkers == 2;
         });
      }


      for (auto& cell : grid) {
         cell.clear();
      }

      // Add particles into grid by index
      for (size_t i{}; i < particles.size(); i++) {
         Vector2 position {particles[i].x, particles[i].y};

         int cellIndex = getCellIndex(position);

         grid[cellIndex].push_back(i);
      }

      for (size_t i{}; i < particles.size(); i++) {
         int cellX = particles[i].x / cellSize;
         int cellY = particles[i].y / cellSize;

         for (int offsetY = -1; offsetY <= 1; offsetY++) {
            for (int offsetX = -1; offsetX <= 1; offsetX++) {
               int neighborX = cellX + offsetX;
               int neighborY = cellY + offsetY;

               // Check if neighbors are within boundaries
               if (neighborX < 0 || neighborX >= gridWidth) continue;
               if (neighborY < 0 || neighborY >= gridHeight) continue;
               
               int neighborCell = neighborY * gridWidth + neighborX;

               for (int other : grid[neighborCell]) {
                  if (i >= other) continue;

                  Particle& self = particles[i];
                  Particle& oth = particles[other];

                  float dx = oth.x - self.x;
                  float dy = oth.y - self.y;

                  float distanceSqrt = dx * dx + dy * dy;

                  int radiusSum = radius + radius;

                  if (distanceSqrt < radiusSum * radiusSum) {
                     float distance = sqrt(distanceSqrt);

                     if (distance == 0) continue;

                     // Normal Collision Vector
                     float nx = dx / distance;
                     float ny = dy / distance;

                     // Position Correction

                     float overlap = radiusSum - distance;
                     float correction = overlap / 2.0f;

                     self.x -= nx * correction;
                     self.y -= ny * correction;

                     oth.x += nx * correction;
                     oth.y += ny * correction;

                     // Relativ Velocity
                     float relVelX = oth.velocityX - self.velocityX;
                     float relVelY = oth.velocityY - self.velocityY;

                     // Relativ Velocity on the Collision Normal
                     float relNorm = relVelX * nx + relVelY * ny;

                     if (relNorm > 0) continue;

                     float impulse = -relNorm;

                     self.velocityX -= impulse * nx;
                     self.velocityY -= impulse * ny;

                     oth.velocityX += impulse * nx;
                     oth.velocityY += impulse * ny;
                  }
               }
            }
         }
      }

      BeginDrawing(); 
 
      ClearBackground(BLACK); 
      DrawText(TextFormat("FPS: %d", GetFPS()), 0, 0, 30, WHITE); 
 
      for (auto& particle : particles) {
         particle.draw();  
      }

      // Drawing Grid
      for (size_t x{}; x < screenWidth; x += cellSize) {
         DrawLine(x, 0, x ,screenHeight, Fade(GRAY, 0.2f));
      }

      for (size_t y{}; y < screenHeight; y += cellSize) {
         DrawLine(0, y, screenWidth , y, Fade(GRAY, 0.2f));
      }
 
      EndDrawing(); 
   } 
 
   {
      std::lock_guard<std::mutex> lock(mutex);
      running = false;
   }

   startWork.notify_all();

   t1.join();
   t2.join();

   CloseWindow();
 
   return 0; 
}