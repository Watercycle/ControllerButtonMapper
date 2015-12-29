// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#include <iostream>
#include <mutex>
#include <functional>
#include <algorithm>
#include <string>
#include "vec2.h"
#include "SFML/Window.hpp"
#include <sstream>
#include <iterator>
#include <iomanip>
#include <atomic>

using namespace std;
using namespace chrono;

typedef atomic<int> aint;
typedef atomic<bool> abool;

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

// printing stl containers
template <typename T>
ostream& operator<< (ostream& out, const vector<T>& v) 
{
    if (v.empty()) return out;

    out << '[';
    copy(v.begin(), v.end(), ostream_iterator<T>(out, ", "));
    out << "\b\b]";

    return out;
}

namespace util
{
    vector<string>& split(const string& s, char delim, vector<string>& elems);
    vector<string> split(const string& str, char delim);
    string removeSpaces(string str);
    string toLower(string str);
    float scale(float val, float oldMin, float oldMax, float newMin, float newMax);
    float normalize(float val);
    vec2 lerp(vec2 start, vec2 end, float percent);
    string toString(const std::wstring& wstr);
    string trim(string str);


    template <typename T>
    T clip(const T& n, const T& lower, const T& upper) {
        return std::max(lower, std::min(n, upper));
    }
}
