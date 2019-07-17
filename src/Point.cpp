#include "Point.h"

Point::Point(dim_t x, dim_t y)
	: m_x(x)
	, m_y(y) {

}

Point::dim_t Point::x() const {
	return m_x;
}

Point::dim_t Point::y() const {
	return m_y;
}

bool operator==(const Point &lhs, const Point &rhs) {
	return lhs.x() == rhs.x() && lhs.y() == rhs.y();
}

bool operator!=(const Point &lhs, const Point &rhs) {
	return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream &stream, const Point &point) {
	stream << point.x() << " " << point.y();
	return stream;
}
