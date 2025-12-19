#pragma once
#include <raylib.h>

class Button
{
public:
    Button();
    Button(const char *imagePath, Vector2 imagePosition, float scale);
    ~Button();
    void Draw(Vector2 mousePos);
    bool isPressed(Vector2 mousePos, bool mousePressed);

    // NEW: Getters for layout math
    float GetWidth() const;
    float GetHeight() const;

    // NEW: Adjust position after creation
    void SetPosition(Vector2 newPos);

    // NEW: Shrink the collision box (0.0 to 1.0) to ignore transparent borders
    void SetHitboxScale(float scaleX, float scaleY);

private:
    Texture2D buttonTexture;
    Vector2 pos;
    float scale;         // Store scale to calculate dimension
    Vector2 hitboxScale; // Store hitbox reduction factor
    bool textureLoaded;
};