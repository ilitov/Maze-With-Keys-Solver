#include <iostream>
#include <stdexcept>
#include <queue>
#include "ImageInfo.h"

//	Set the width of the keys
const int ImageInfo::KEY_WIDTH = 20;

ImageInfo::ImageInfo(const Image &img)
	: m_image(img)
	, m_start(Point{ -1, -1 }) {

}

void ImageInfo::analyzeImage() {
	//	Vector of visited pixels
	std::vector<bool> visited(m_image.size(), false);

	for (Point::dim_t row = 0, height = m_image.height(); row < height; ++row) {
		for (Point::dim_t col = 0, width = m_image.width(); col < width; ++col) {
			if (visited[row * m_image.width() + col]) {
				continue;
			}

			//	The color of the current pixel
			const auto &pxl = m_image.pixel(Point{ row, col });

			//	Add keys
			if (color(pxl) && isKey(pxl, m_image, row, col, visited)) {
				m_keys[pxl].push_back(Point{ row + ImageInfo::KEY_WIDTH / 2,col + ImageInfo::KEY_WIDTH / 2 });
			}
			//	Find the starting point
			else if (m_start.x() == -1 && m_start.y() == -1 && pxl == Pixel::START) {
				m_start = Point{ row, col };
			}
			//	Add ending points
			else if (pxl == Pixel::END) {
				m_ends.push_back(Point{ row, col });

				//	Fill the ending zone, because we do not want 
				//	to add several pixels for the same figure
				fillZone(Point{ row, col }, m_image, visited);
			}
		}
	}

	if (m_start.x() == -1 || m_start.y() == -1) {
		throw std::logic_error("Starting point was not found!");
	}
}

void ImageInfo::saveImage() const {
	//	Construct the output path
	std::string output = m_image.path();

	while (output.back() != '.') {
		output.pop_back();
	}

	output.pop_back();
	output.append("_output.bmp");

	m_image.saveImage(output);
}

Image& ImageInfo::getImage() {
	return const_cast<Image&>(static_cast<const ImageInfo&>(*this).getImage());
}

const Image& ImageInfo::getImage() const {
	return m_image;
}

const Point ImageInfo::startPoint() const {
	return m_start;
}

const ImageInfo::KeysContainer& ImageInfo::getKeys() const {
	return m_keys;
}

const ImageInfo::EndsContainer& ImageInfo::getEnds() const {
	return m_ends;
}

bool ImageInfo::color(const Pixel::pxl_t &pixel) {
	return  pixel != Pixel::WHITE &&
			pixel != Pixel::BLACK &&
			pixel != Pixel::START &&
			pixel != Pixel::END;
}

bool ImageInfo::isKey(const Pixel::pxl_t &color, const Image &image, Point::dim_t row, Point::dim_t col, std::vector<bool> &visited) {
	//	Check if the key is contained entirely in the image
	if (row + KEY_WIDTH >= image.height() || col + KEY_WIDTH >= image.width()) {
		return false;
	}

	//	Check the filling of the tested area(must contain only pixels with the same color)
	for (Point::dim_t x = row; x < row + KEY_WIDTH; ++x) {
		for (Point::dim_t y = col; y < col + KEY_WIDTH; ++y) {
			if (image.pixel(Point{ x, y }) != color) {
				return false;
			}

			visited[x * image.width() + y] = true;
		}
	}

	//	Check the outer square frame(every key is exactly 20x20)

	//	Top row
	for (Point::dim_t i = 0; row > 0 && i <= KEY_WIDTH + 1; ++i) {
		if (col > 0 && col + i <= image.width() && image.pixel(Point{ row - 1, col + i - 1 }) == color) {
			return false;
		}
	}

	//	Bottom row
	for (Point::dim_t i = 0; i <= KEY_WIDTH + 1; ++i) {
		if (col > 0 && col + i <= image.width() && image.pixel(Point{ row + KEY_WIDTH, col + i - 1 }) == color) {
			return false;
		}
	}

	//	Left column
	for (Point::dim_t i = 0; col > 0 && i < KEY_WIDTH; ++i) {
		if (image.pixel(Point{ row + i, col - 1 }) == color) {
			return false;
		}
	}

	//	Right column
	for (Point::dim_t i = 0; i < KEY_WIDTH; ++i) {
		if (image.pixel(Point{ row + i, col + KEY_WIDTH }) == color) {
			return false;
		}
	}

	return true;
}

void ImageInfo::fillZone(const Point &pos, const Image &img, std::vector<bool> &visited) {
	std::queue<Point> queue;
	queue.push(pos);

	const auto &color = img.pixel(pos);
	visited[pos.x() * img.width() + pos.y()] = true;

	while (!queue.empty()) {
		auto curr = queue.front();
		queue.pop();

		const auto &adjacent = getAdjacent(curr);
		for (const auto &adj : adjacent) {
			if (!visited[adj.x() * img.width() + adj.y()] && img.pixel(adj) == color) {
				visited[adj.x() * img.width() + adj.y()] = true;
				queue.push(adj);
			}
		}
	}
}

const std::vector<Point> ImageInfo::getAdjacent(const Point &point) const {
	const size_t nSize = 8;
	const Point::dim_t dir[nSize][2] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0},{1,1}, {0,1},{-1,1} };

	std::vector<Point> result;

	for (size_t i = 0; i < nSize; ++i) {
		Point::dim_t nextX = point.x() + dir[i][0];
		Point::dim_t nextY = point.y() + dir[i][1];

		if (nextX >= 0 && nextX < m_image.height() && nextY >= 0 && nextY < m_image.width()) {
			result.push_back(Point{ nextX, nextY });
		}
	}

	return result;
}
