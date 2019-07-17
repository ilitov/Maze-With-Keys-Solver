#pragma once
#ifndef IMAGE_ANALYZE_CLASS_HEADER
#define IMAGE_ANALYZE_CLASS_HEADER

#include <unordered_map>
#include <vector>

#include "Image.h"

class ImageInfo {
public:
	static const int KEY_WIDTH;

	//	For every key color there is a vector of points and each point in that vector
	//	describes the position of a key with that color
	using KeysContainer = std::unordered_map<Pixel::pxl_t, std::vector<Point>>;

	//	Vector of points that keeps the positions of the end zone(s)
	using EndsContainer = std::vector<Point>;

public:
	ImageInfo(const Image &img);
	ImageInfo(const ImageInfo &r) = default;
	ImageInfo& operator=(const ImageInfo &rhs) = default;
	~ImageInfo() = default;

public:
	void analyzeImage();
	void saveImage() const;

	//	Returns *true* if the pixel is colored
	static bool color(const Pixel::pxl_t &pixel);

public:
	Image& getImage();
	const Image& getImage() const;

	const Point startPoint() const;

	const KeysContainer& getKeys() const;
	const EndsContainer& getEnds() const;

private:
	//	Returns *true* if the zone in that position is a key
	static bool isKey(const Pixel::pxl_t &pixel, const Image &img, Point::dim_t row, Point::dim_t col, std::vector<bool> &visited);

	const std::vector<Point> getAdjacent(const Point &point) const;
	void fillZone(const Point &pos, const Image &img, std::vector<bool> &visited);

private:
	Image m_image;
	Point m_start;
	KeysContainer m_keys;
	EndsContainer m_ends;
};

#endif // !IMAGE_ANALYZE_CLASS_HEADER
