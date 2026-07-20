#pragma once
#include "utils.hpp"

constexpr float PI = static_cast<float>(3.1415926535);

class Camera {
    public:
        Camera(int width, int height): aspectRatio{static_cast<float>(width) / height} {}
        void rotateCamera(float dx, float dy) {
            rotX += dy / sensitivityDamper;
            rotY += dx / sensitivityDamper;

            // Naturally bounded vertical head movement
            if (rotX < -PI / 2 + verticalRotationBuffer) rotX = -PI / 2 + verticalRotationBuffer;
            else if (rotX > PI / 2 - verticalRotationBuffer) rotX = PI / 2 - verticalRotationBuffer;

            // Horizontal movement value boundaries
            if (rotY >= 2 * PI) rotY -= 2 * PI;
            else if (rotY <= -2 * PI) rotY += 2 * PI;
        }

        // Movement: vertically absolute and horizontally relative
        void move(float dx, float dy, float dz) {float sinRotY = std::sin(getRotY()); float cosRotY = std::cos(getRotY());
                                                 x += dx / NORM(movementDamper) * cosRotY + dz / NORM(movementDamper) * sinRotY;
                                                 y += dy / NORM(movementDamper);
                                                 z += -dx / NORM(movementDamper) * sinRotY + dz / NORM(movementDamper) * cosRotY;}

        float getX() const {return x;}
        float getY() const {return y;}
        float getZ() const {return z;}

        float getRotX() const {return rotX;}
        float getRotY() const {return rotY;}

        float getAspectRatio() const {return aspectRatio;}
        float getFOV() const {return FOV;}

        void print() const {std::cout << rotX << ", " << rotY << "\n";
                      std::cout << x << ", " << y << ", " << z << "\n";}
    private:
        float x = 0, y = 0, z = 0;
        float rotX = 0, rotY = 0;
        float aspectRatio = 0, FOV = 45;

        float movementDamper = 0.1f;
        float sensitivityDamper = 400.0f;
        float verticalRotationBuffer = 0.01f;
};