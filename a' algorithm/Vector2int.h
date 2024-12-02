#pragma once
#include <cmath>
class Vector2Int
{
public:
    int posx;
    int posy;

public:
    Vector2Int() : posx(0), posy(0) {}
    Vector2Int(int x, int y) { posx = x; posy = y; }
    ~Vector2Int() {}

    static Vector2Int up() { return Vector2Int(0, 1); }
    static Vector2Int down() { return Vector2Int(0, -1); }
    static Vector2Int left() { return Vector2Int(-1, 0); }
    static Vector2Int right() { return Vector2Int(+1, 0); }

    Vector2Int operator+(const Vector2Int& other) const {
        return Vector2Int(posx + other.posx, posy + other.posy);
    }
    Vector2Int operator-(const Vector2Int& other) const {
        return Vector2Int(posx - other.posx, posy - other.posy);
    }
    Vector2Int& operator+=(const Vector2Int& other) {
        posx += other.posx;
        posy += other.posy;
        return *this;
    }
    double Magnitude() const { return std::sqrt(sqrMagnitude()); }
    int sqrMagnitude() const { return posx * posx + posy * posy; }
    int cellDistFromZero() const { return std::abs(posx) + std::abs(posy); }
};