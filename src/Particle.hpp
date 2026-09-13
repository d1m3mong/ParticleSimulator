#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <iostream>
#include "raylib.h"

#endif

constexpr int radius = 1;
constexpr float gravity = 500.0f;

constexpr float friction = 0.99f;
constexpr float attraction = 0.9f;

struct Particle
{
    float x, y, velocityX, velocityY;

    void draw() {
        DrawCircle(this->x, this->y, radius, WHITE);
    }

    void move() {
        velocityY += gravity * GetFrameTime();
        //velocityX *= friction;

        x += velocityX * GetFrameTime();
        y += velocityY * GetFrameTime();

        if (x >= 1280 - radius) {
            x = 1280 - radius;
            velocityX = -velocityX;
        }

        if (x <= radius) {
            x = radius;
            velocityX = -velocityX;
        }

        if (y >= 720 - radius) {
            y = 720 - radius;
            velocityY = -velocityY * attraction;
        }

        if (y <= radius) {
            y = radius;
            velocityY = -velocityY * attraction;
        }
    }
};
