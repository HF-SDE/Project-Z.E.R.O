#pragma once

#include <string>
#include <iostream>

// Minimal Arduino String replacement for unit tests
class String
{
    std::string s;

public:
    String() : s() {}
    String(const char *c) : s(c ? c : "") {}
    String(const std::string &other) : s(other) {}
    String(const String &other) : s(other.s) {}
    explicit String(int v) : s(std::to_string(v)) {}

    const char *c_str() const { return s.c_str(); }
    size_t length() const { return s.size(); }
    bool isEmpty() const { return s.empty(); }
    size_t size() const { return s.size(); }
    void reserve(size_t n) { s.reserve(n); }
    void clear() { s.clear(); }

    String &operator+=(const String &o)
    {
        s += o.s;
        return *this;
    }

    friend String operator+(const String &a, const String &b)
    {
        return String(a.s + b.s);
    }

    friend String operator+(const String &a, const char *b)
    {
        return String(a.s + std::string(b ? b : ""));
    }

    friend String operator+(const char *a, const String &b)
    {
        return String(std::string(a ? a : "") + b.s);
    }

    operator std::string() const { return s; }
};

// Minimal Serial mock
struct _Serial
{
    void begin(int) {}
    void println(const String &m) { std::cout << m.c_str() << std::endl; }
    void println(const char *m) { std::cout << (m ? m : "") << std::endl; }
    void print(const String &m) { std::cout << m.c_str(); }
    void print(const char *m) { std::cout << (m ? m : ""); }
    void printf(const char *fmt, ...)
    {
        // Very small helper - not formatting, just forward as println
        std::cout << (fmt ? fmt : "") << std::endl;
    }
};

extern _Serial Serial;

// Minimal Flash string helper
struct __FlashStringHelper
{
};

// Provide a simple F() macro that returns a pointer to __FlashStringHelper
#define F(x) (reinterpret_cast<const __FlashStringHelper *>(x))
