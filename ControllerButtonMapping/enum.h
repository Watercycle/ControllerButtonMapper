#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <ostream>

// External Macro Mapping Utility
// =======================================================================================================

/*
* Copyright (C) 2012 William Swanson
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies
* of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the names of the authors or
* their institutions shall not be used in advertising or otherwise to
* promote the sale, use or other dealings in this Software without
* prior written authorization from the authors.
*/

// Github Page: https ://github.com/swansontec/map-macro

// The MAP macro is a recursive macro that currently has a maximum depth of 365.  So you can 
// only have a maximum of 365 enums while  using the macro.  Note however, that the c++11 standard 
// caps the compiler macro parameter count at n = 256 and Microsoft Visual Studio at n = 127 (Microsoft 
// being special like usual).  Though, due to the setup calls you are actually only able to have 'n - 4'
// enums before you get some ludicrous compiler error.

// Stupid visual studio compiler tries to chunk the VA_ARGS together.  This corrects that.
#define EXPAND( x ) x

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0 (EVAL0 (EVAL0 (__VA_ARGS__)))
#define EVAL2(...) EVAL1 (EVAL1 (EVAL1 (__VA_ARGS__)))
#define EVAL3(...) EVAL2 (EVAL2 (EVAL2 (__VA_ARGS__)))
#define EVAL4(...) EVAL3 (EVAL3 (EVAL3 (__VA_ARGS__)))
#define EVAL(...)  EVAL4 (EVAL4 (EVAL4 (__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT

#define MAP_GET_END() 0, MAP_END
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) EXPAND( MAP_NEXT0 (test, next, 0) )
#define MAP_NEXT(test, next)  EXPAND( MAP_NEXT1 (MAP_GET_END test, next) )

#define MAP0(fn, x, peek, ...) fn(x) MAP_NEXT (peek, MAP1) (fn, peek,  __VA_ARGS__ )
#define MAP1(fn, x, peek, ...) fn(x) MAP_NEXT (peek, MAP0) (fn, peek,  __VA_ARGS__ )

/// A variadric macro mapping function that returns the results of the expanded macro.
/// Example:
/// #define DECLARE_STRING(str) string str = #str;
/// MAP(DECLARE_STRING, Aqua, Tera, Ventus)
/// cout << Tera << endl; // output: Tera
#define MAP(fn, ...) EVAL (MAP1 (fn, __VA_ARGS__, (), 0))


// Base Enum Class Type (helps unify generic parts and keep some kind of type safety)
// =======================================================================================================

template <typename EType>
class Enum
{
protected:
    typedef unsigned _UnderlyingType;

public:
    static std::unique_ptr<bool> _alreadyInitialized;

    // class implicitly casts to the underlying enum type (so that it can work with switch statements)
    operator _UnderlyingType() const
    {
        return static_cast<_UnderlyingType>(_enumValue);
    }

    // implicit conversion to string
    operator std::string() const
    {
        return _enumToString->at(_enumValue);
    }

    static void _initialize()
    {
        // Gurantees that our static variables are initialized BEFORE main and BEFORE the child enums try to
        // populate anything.  Otherwise, the static variables will be initialized at some point after main.
        // This would cause static methods like fromString to run before the enum was prepared.
        _iterationVector = std::make_unique<std::vector<EType>>();
        _enumToString = std::make_unique<std::vector<std::string>>();
        _stringToEnum = std::make_unique<std::unordered_map<std::string, _UnderlyingType>>();
    }

    // Uses underlying type to explicitly downcast back down to the specific type.
    static EType from(const std::string& str)
    {
        const string safeStr = util::toLower(str);
        bool found = _stringToEnum->count(safeStr) > 0;
        return (found) ?
            EType(static_cast<typename EType::_Type>(_stringToEnum->at(safeStr))) :
            EType(static_cast<typename EType::_Type>(NULL));
    }

    static EType from(_UnderlyingType type)
    {
        return EType(static_cast<typename EType::_Type>(type));
    }

    /// Returns true if the current enum is one of the provided enums.
    bool in(const std::unordered_set<EType>& candidates)
    {
        return candidates.count(EType(static_cast<typename EType::_Type>(_enumValue))) > 0;
    }

    bool isDefined() const
    {
        return _enumValue != NULL;
    }

    static bool contains(const std::string& str)
    {
        return _stringToEnum->count(util::toLower(str)) > 0;
    }

    /// Returns an iterable vector of all of the enums.
    static std::vector<EType> values()
    {
        return *_iterationVector.get();
    }

    /// The undefined element is *not* included as part of size.
    static size_t size()
    {
        return _iterationVector->size();
    }

    // OS stream should print the string, not the underlying type.
    friend std::ostream& operator<< (std::ostream& os, const Enum& type)
    {
        os << type._enumToString->at(type._enumValue);
        return os;
    }

protected:
    // Enum should never be initialized
    virtual ~Enum() = 0;

    // used by Enum to downcast to the template type
    explicit Enum(_UnderlyingType type) : _enumValue(type) {}
        
    _UnderlyingType _enumValue;

    static std::unique_ptr<std::vector<EType>> _iterationVector; // fast iteration
    static std::unique_ptr<std::vector<std::string>> _enumToString; // O(1) enum to string
    static std::unique_ptr<std::unordered_map<std::string, _UnderlyingType>> _stringToEnum; // O(1) string to enum

    static void _add(_UnderlyingType type, const std::string& str)
    {
        // ignore the undefined type
        if (type != NULL) _iterationVector->push_back(EType(static_cast<typename EType::_Type>(type)));

        _enumToString->push_back(util::toLower(str));
        _stringToEnum->insert({ util::toLower(str), type });
    }
};

// Static Member Declarations
// =======================================================================================================

template <typename EType> Enum<EType>::~Enum() {}
template <typename EType> std::unique_ptr<std::vector<EType>> Enum<EType>::_iterationVector;
template <typename EType> std::unique_ptr<std::vector<std::string>> Enum<EType>::_enumToString;
template <typename EType> std::unique_ptr<std::unordered_map<std::string, typename Enum<EType>::_UnderlyingType>> Enum<EType>::_stringToEnum;
template <typename EType> std::unique_ptr<bool> Enum<EType>::_alreadyInitialized;

// Convenience Macros (so users don't have to type out base classes and its initializer)
// =======================================================================================================

#define STRING_TO_ENUM(x) _add(x, #x);
#define UNDEFINED_ENUM Undefined


/// Used at the bottom of an enum declaration to generate the necessary initializers for that enum.
/// qualifiedEnumName is necessary because 'namespace std' can not be used within a namespace.
#define GENERATE_ENUM_INITIALIZER(enumName, enumNameQualified)                              \
/* A separate class is used to force the initialization of the static classes before main */\
struct enumName ## Initializer                                                              \
{                                                                                           \
    enumName ## Initializer()                                                               \
    {                                                                                       \
        /* Multiple includes result in multiple initializations.  Prevent that. */          \
        if (Enum<enumNameQualified>::_alreadyInitialized.get() != nullptr) return;          \
        Enum<enumNameQualified>::_alreadyInitialized = std::make_unique<bool>(true);        \
                                                                                            \
        /* The Enum base class needs to be initialized first before being populated */      \
        Enum<enumNameQualified>::_initialize();                                             \
        enumNameQualified::_initialize();                                                   \
    }                                                                                       \
};                                                                                          \
                                                                                            \
/* triggers the Initializer struct's constructor */                                         \
static enumName ## Initializer _run_initialize_before_main_ ## enumName;                    \
                                                                                            \
/* Add support for STL functions so we can, for example, use our enum in a map */           \
namespace std                                                                               \
{                                                                                           \
    template <> struct hash<enumNameQualified>                                              \
    {                                                                                       \
        size_t operator()(const enumNameQualified& x) const { return x._enumValue; }        \
    };                                                                                      \
}


#define GENERATE_ENUM_BODY(enumName)                                                        \
    /* Type safe constructor and assignment */                                              \
    enumName(_Type type = UNDEFINED_ENUM) : Enum(type) {}                                   \
                                                                                            \
    /* Expose _enumValue for the hash function. */                                          \
    friend struct std::hash<enumName>;


/// Creates an enum with a qualified namespace as the second parameter.  This is used to circumvent
/// the fact that the std implementations must be defined in the global namespace.
#define CREATE_ENUM_NS(enumName, enumNameQualified, ...)                                    \
struct enumName : Enum<enumName>                                                            \
{                                                                                           \
    enum _Type { UNDEFINED_ENUM, __VA_ARGS__ };                                             \
                                                                                            \
    /* Called before main, but after Enum's initialize method */                            \
    static void _initialize()                                                               \
    {                                                                                       \
        _add(UNDEFINED_ENUM, "<undefined>");                                                \
        MAP(STRING_TO_ENUM, __VA_ARGS__)                                                    \
    }                                                                                       \
                                                                                            \
    GENERATE_ENUM_BODY(enumName)                                                            \
};                                                                                          \
                                                                                            \
GENERATE_ENUM_INITIALIZER(enumName, enumNameQualified)


/// Convenience macro.  Creates an enum in the global namespace
#define CREATE_GLOBAL_ENUM(enumName, ...) CREATE_ENUM_NS(enumName, enumName, __VA_ARGS__)
