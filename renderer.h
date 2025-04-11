#pragma once

class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void initialize(int rows, int cols) = 0;
    virtual void render() = 0;
    virtual void drawTank(int y, int x, int playerNumber) = 0;
    virtual void drawWall(int y, int x) = 0;
    virtual void drawMine(int y, int x) = 0;
};
