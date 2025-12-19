#include "buttons.hpp"

Button::Button()
{
    buttonTexture = {0};
    pos = {0, 0};
    scale = 1.0f;
    hitboxScale = {1.0f, 1.0f}; // Default: Full image is clickable
    textureLoaded = false;
}

Button::Button(const char *imagePath, Vector2 imagePosition, float scaleFactor)
{
    Image image = LoadImage(imagePath);

    int originalW = image.width;
    int originalH = image.height;

    // Resize permanently to save VRAM and processing
    int newWidth = static_cast<int>(originalW * scaleFactor);
    int newHeight = static_cast<int>(originalH * scaleFactor);

    ImageResize(&image, newWidth, newHeight);
    buttonTexture = LoadTextureFromImage(image);
    UnloadImage(image);

    pos = imagePosition;
    scale = 1.0f;               // Since we resized the image, base scale is now 1.0
    hitboxScale = {0.8f, 0.7f}; // DEFAULT FIX: Shrink hitbox to 80% width, 70% height to ignore padding
    textureLoaded = true;
}

Button::~Button()
{
    if (textureLoaded)
    {
        UnloadTexture(buttonTexture);
    }
}

float Button::GetWidth() const
{
    return (float)buttonTexture.width;
}

float Button::GetHeight() const
{
    return (float)buttonTexture.height;
}

void Button::SetPosition(Vector2 newPos)
{
    pos = newPos;
}

void Button::SetHitboxScale(float scaleX, float scaleY)
{
    hitboxScale = {scaleX, scaleY};
}

void Button::Draw(Vector2 mousePos)
{
    if (!textureLoaded)
        return;

    // Calculate the centered, smaller hitbox
    float width = (float)buttonTexture.width;
    float height = (float)buttonTexture.height;

    float hitW = width * hitboxScale.x;
    float hitH = height * hitboxScale.y;
    float hitX = pos.x + (width - hitW) / 2.0f;
    float hitY = pos.y + (height - hitH) / 2.0f;

    Rectangle hitRect = {hitX, hitY, hitW, hitH};

    bool isHovered = CheckCollisionPointRec(mousePos, hitRect);
    Color tint = WHITE;
    float drawScale = 1.0f;

    if (isHovered)
    {
        tint = GRAY;
        drawScale = 1.05f;
    }

    // Draw centering correction for scale effect
    // When scaling up by 1.05, we shift position slightly to keep it centered
    float drawX = pos.x - (width * (drawScale - 1.0f) / 2.0f);
    float drawY = pos.y - (height * (drawScale - 1.0f) / 2.0f);

    DrawTextureEx(buttonTexture, {drawX, drawY}, 0.0f, drawScale, tint);

    // Debug: Uncomment to see the hitbox
    // DrawRectangleLinesEx(hitRect, 2.0f, RED);
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed)
{
    if (!textureLoaded)
        return false;

    float width = (float)buttonTexture.width;
    float height = (float)buttonTexture.height;

    float hitW = width * hitboxScale.x;
    float hitH = height * hitboxScale.y;
    float hitX = pos.x + (width - hitW) / 2.0f;
    float hitY = pos.y + (height - hitH) / 2.0f;

    Rectangle hitRect = {hitX, hitY, hitW, hitH};

    if (CheckCollisionPointRec(mousePos, hitRect) && mousePressed)
    {
        return true;
    }
    return false;
}