#pragma once

constexpr int DISPLAY_WIDTH = 1280;
constexpr int DISPLAY_HEIGHT = 720;
constexpr int DISPLAY_SCALE = 1;

// The global GameState structure
// Don't try and store GameObject references or ids in here unless you can 
// guarantee they will never change or get deleted
struct GameState
{
    float time{ 0 };
    int score{ 0 };
    int difficulty{ 1 };
};

