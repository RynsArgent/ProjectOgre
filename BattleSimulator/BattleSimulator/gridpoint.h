#ifndef __GRIDPOINT_H__
#define __GRIDPOINT_H__

#include "pch.h"

struct GridPoint
{
    int x;
    int y;
    
    GridPoint() : x(0), y(0) {}
    GridPoint(int x, int y) : x(x), y(y) {}
    
    bool operator==(const GridPoint & rhs) const { return x == rhs.x && y == rhs.y; }
    bool operator!=(const GridPoint & rhs) const { return x != rhs.x || y != rhs.y; }
    
    GridPoint operator+(const GridPoint & rhs) const { return GridPoint(x + rhs.x, y + rhs.y); }
    GridPoint operator-(const GridPoint & rhs) const { return GridPoint(x - rhs.x, y - rhs.y); }
    GridPoint operator*(int value) const { return GridPoint(x * value, y * value); }
    GridPoint operator/(int value) const { return GridPoint(x / value, y / value); }
    GridPoint operator+=(const GridPoint & rhs) { x = x + rhs.x; y = y + rhs.y; return *this; }
    GridPoint operator-=(const GridPoint & rhs) { x = x - rhs.x; y = y - rhs.y; return *this; }
    
    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }
    
    ~GridPoint() {}
};

#endif
