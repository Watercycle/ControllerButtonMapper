#include "stdafx.h"
#include "vec2.h"

string vec2::toString()
{
    return "(" + to_string(x) + ", " + to_string(y) + ")";
}

ostream& operator<< (ostream& out, const vec2& vec)
{
    out << '(';
    out << vec.x << ", " << vec.y;
    out << ")";

    return out;
}