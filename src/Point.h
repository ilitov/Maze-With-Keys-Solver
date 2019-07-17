#pragma once
#ifndef POINT_CLASS_HEADER
#define POINT_CLASS_HEADER

#include <fstream>

class Point {
public:
	using dim_t = int;

public:
	explicit Point(dim_t x, dim_t y);
	Point() = default;
	Point(const Point &r) = default;
	Point& operator=(const Point &rhs) = default;
	~Point() = default;

public:
	dim_t x() const;
	dim_t y() const;

private:
	dim_t m_x;
	dim_t m_y;
};

bool operator==(const Point &lhs, const Point &rhs);
bool operator!=(const Point &lhs, const Point &rhs);
std::ostream& operator<<(std::ostream &stream, const Point &point);

#endif // !POINT_CLASS_HEADER
