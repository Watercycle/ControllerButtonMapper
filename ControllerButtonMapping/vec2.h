#pragma once
#include <ostream>

struct vec2
{
    float x, y;
    vec2() : x(0), y(0) {}
    vec2(float x, float y) : x(x), y(y) {}

    std::string toString();

    float lengthSquared() const { return x * x + y * y; }
    float length() const { return sqrt(x * x + y * y); }
    vec2 operator+ (const vec2& other) const { return vec2(x + other.x, y + other.y); }
    vec2 operator- (const vec2& other) const { return vec2(x - other.x, y - other.y); }
    vec2 operator* (float s) const { return vec2(s * x, s * y); }
    vec2 operator* (const vec2& other) const { return vec2(x * other.x, y * other.y); }
    vec2 operator/ (float s) const { return vec2(x / s, y / s); }
    vec2 operator/ (const vec2& other) const { return vec2(x / other.x, y / other.y); }
    bool operator== (const vec2& other) const { return x == other.x && y == other.y; }
    bool operator!= (const vec2& other) const { return x != other.x || y != other.y; }
    vec2 operator- () const { return vec2(-x, -y); }

    vec2& operator+= (const vec2& other) 
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    vec2& operator-= (const vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vec2& operator*= (float s)
    {
        x *= s; 
        y *= s; 
        return *this;
    }

    vec2& operator*= (const vec2& other)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    vec2& operator/= (float s) 
    {
        x /= s; 
        y /= s; 
        return *this;
    }

    vec2 normalized() const
    {
        vec2 newVec(x, y);
        return newVec / newVec.length();
    }

    vec2& normalize()
    {
        if (length() == 0) return *this;
        *this *= (1.0f / length());
        return *this;
    }

    vec2& clip(const vec2& min, const vec2& max)
    {
        if (x < min.x) x = min.x;
        if (y < min.y) y = min.y;
        if (x > max.x) x = max.x;
        if (y > max.y) y = max.y;
        return *this;
    }
    
    vec2& abs()
    {
        x = std::abs(x);
        y = std::abs(y);
        return *this;
    }

    float dist(const vec2& other)
    {
        float dx = other.x - x;
        float dy = other.y - y;
        return std::sqrt(dx*dx + dy*dy);
    }

    vec2& negate()
    {
        x = -x;
        y = -y;
        return *this;
    }

    vec2 negated()
    {
        return vec2(-x, -y);
    }

    friend std::ostream& operator<< (std::ostream& out, const vec2& vec);
};