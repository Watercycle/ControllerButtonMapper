// stdafx.cpp : source file that includes just the standard includes
// ControllerButtonMapping.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
vector<string>& util::split(const string& s, char delim, vector<string>& elems)
{
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) 
    {
        elems.push_back(item);
    }

    return elems;
}

vector<string> util::split(const string& str, char delim)
{
    vector<string> tokens;
    split(str, delim, tokens);
    auto end = remove_if(tokens.begin(), tokens.end(), [](string& s) {return s.empty();});
    return vector<string>(tokens.begin(), end);
}

string util::removeSpaces(string str)
{
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

string util::toLower(string str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

float util::scale(float val, float oldMin, float oldMax, float newMin, float newMax)
{
    return ((val - oldMin) / (oldMax - oldMin)) * (newMax - newMin) + newMin;
}

float util::normalize(float val)
{
    return (val >= 0.0f) ? 1.0f : -1.0f;
}

vec2 util::lerp(vec2 start, vec2 end, float percent)
{
    return start + (end - start) * percent;
}
