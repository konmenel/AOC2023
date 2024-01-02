#ifndef RECYCLES_H
#define RECYCLES_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <complex>


#ifdef DEBUG
    #define debug_println(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__) << std::endl
    #define debug_print(fmt, ...) std::cout << std::format((fmt), ##__VA_ARGS__)
#else
    #define debug_println(fmt, ...)
    #define debug_print(fmt, ...)
#endif


namespace utils{

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef size_t   usize;

typedef std::complex<i32> Pos;
typedef std::complex<i32> Dir;
typedef std::complex<i32> Rot;

const Dir NORTH( 0,-1);
const Dir SOUTH( 0, 1);
const Dir WEST (-1, 0);
const Dir EAST ( 1, 0);

template<typename T> 
class Grid {
public:
    usize rows;
    usize cols;

    Grid(usize rows_, usize cols_, const T &value) : rows(rows_), cols(cols_), m_data(rows*cols, value) {}

    Grid<T>& operator=(const Grid<T> &g) {
        rows = g.rows;
        cols = g.cols;
        m_data = g.m_data;
        return *this;
    }

    i32 count(const T& t) const {
        i32 count = 0;
        for (usize y = 0; y < rows; y++) {
            for (usize x = 0; x < cols; x++) {
                if (at(x, y) == t) count++;
            }
        }
        return count;
    }

    T& at(usize x, usize y) {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    const T& at(usize x, usize y) const {
        if (x >= cols) {
            debug_println("x {} exceeds total number of columns({})!", x, cols-1);
        } else if (y >= rows) {
            debug_println("y {} exceeds total number of rows({})!", y, rows-1);
        }
        return m_data[y*cols + x];
    }

    T& operator()(usize x, usize y) { return at(x, y); }

    T& operator()(Pos pos) { return at(pos.real(), pos.imag()); }

    const T& operator()(usize x, usize y) const { return at(x, y); }

    const T& operator()(Pos pos) const { return at(pos.real(), pos.imag()); }

    bool operator==(const Grid<T> &rhs) const {
        if (rhs.cols != cols || rhs.rows != rows)
            return false;

        for (usize y = 0; y < rows; y++) {
            for (usize x = 0; x < cols; x++) {
                if (at(x, y) != rhs(x, y))
                    return false;
            }
        }
        return true;
    }

    inline bool isOutOfBound(i32 x, i32 y) const {
        return x < 0 || y < 0 || x >= (i32)cols || y >= (i32)rows;
    }

    bool isOutOfBound(const Pos &pos) const {
        return isOutOfBound(pos.real(), pos.imag());
    }

    std::string toString() const {
        std::stringstream ss;
        for (size_t y = 0; y < rows; y++) {
            for (size_t x = 0; x < cols; x++) {
                ss << at(x, y);
            }
            ss << "\n";
        }
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid<T>& grid) {
        os << grid.toString();
        return os;
    }

private:
    std::vector<T> m_data;
};

} // namespace utils
#endif