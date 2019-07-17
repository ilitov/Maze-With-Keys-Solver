#pragma once
#ifndef IMAGE_CLASS_HEADER
#define IMAGE_CLASS_HEADER

#include <vector>
#include <string>

#include "Pixel.h"	//describes RGB value of each pixel
#include "Point.h"	//describes the position of each pixel on the grid

class Image {
public:
	Image(const std::string &path);
	Image(const Image &r) = default;
	Image& operator=(const Image &rhs) = default;
	~Image() = default;

public:
	bool loadImage();
	bool saveImage(const std::string &path) const;

	size_t size() const;
	const std::string& path() const;

	Point::dim_t width() const;
	Point::dim_t height() const;

	const Pixel::pxl_t& pixel(const Point &position) const;
	void setPixel(const Point &position, const Pixel::pxl_t &pxl);

private:
	std::string m_imagePath;
	std::vector<Pixel::pxl_t> m_data;
	Point::dim_t m_width;
	Point::dim_t m_height;
};

#endif // !IMAGE_CLASS_HEADER
