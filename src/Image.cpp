#include <iostream>
#include <cstdint>
#include <fstream>
#include "Image.h"

Image::Image(const std::string &path)
	: m_imagePath(path) {

}

bool Image::loadImage() {
	if (m_imagePath.empty()) {
		std::cout << "There is no set path!\n";
		return false;
	}

	std::ifstream ifile(m_imagePath, std::ios::binary);
	if (!ifile) {
		std::cout << "Cannot open the image path!\n";
		return false;
	}

	auto clearAndClose = [&ifile]() {
		ifile.clear();
		ifile.close();
	};

	// Check for "BM" in the header of the.bmp file
	const uint32_t headerBM = 0x4D42;

	uint16_t buffer16 = 0;
	uint32_t buffer32 = 0;

	//Check "BM"
	ifile.read((char*)&buffer16, sizeof(buffer16));
	if (buffer16 != headerBM) {
		std::cout << "Invalid image type!\n";
		clearAndClose();
		return false;
	}

	//Read the size of the bmp file
	uint32_t fileSizeInBytes = 0;
	ifile.read((char *)&fileSizeInBytes, sizeof(fileSizeInBytes));

	//Seekg some useless bytes
	ifile.seekg(10, std::ios::beg);

	//The distance to the pixels
	uint32_t imageDataOffset = 0;
	ifile.read((char*)&imageDataOffset, sizeof(imageDataOffset));

	//Skip bytes to width/height
	ifile.seekg(18, std::ios::beg);

	//Buffer variables for imageWidth and imageHeight
	uint32_t width = 0, height = 0;
	ifile.read((char*)&width, sizeof(width));
	ifile.read((char*)&height, sizeof(height));

	if (width == 0 || height == 0) {
		std::cout << "Invalid image size!\n";
		clearAndClose();
		return false;
	}

	//Check if there is 1 plane
	uint16_t numberOfPlanes = 0;
	ifile.read((char *)&numberOfPlanes, sizeof(numberOfPlanes));
	if (numberOfPlanes != 1) {
		std::cout << "Invalid number of planes!\n";
		clearAndClose();
		return false;
	}

	//Check if there are 24 bits per pixel
	uint16_t bitsPerPixel;
	ifile.read((char*)&bitsPerPixel, sizeof(bitsPerPixel));
	if (bitsPerPixel != 24) {
		std::cout << "No 24 bits on pixel!\n";
		clearAndClose();
		return false;
	}

	//Check for compression
	ifile.read((char*)&buffer32, sizeof(buffer32));
	if (buffer32 != 0) {
		std::cout << "The image is compressed!\n";
		clearAndClose();
		return false;
	}

	//Seekg to the beginning ot the pixels
	ifile.seekg(imageDataOffset, std::ios::beg);

	//Reserve enough space
	m_data.resize(width * height);

	//There is a padding
	//BMP is aligned at 4 bytes
	uint32_t padding = (4 - (width * 3) % 4) % 4;

	for (size_t row = 0; row < height; ++row) {
		for (size_t col = 0; col < width && ifile; ++col) {
			size_t RGB = 0;

			ifile.read((char*)&RGB, 3);

			/*Pixel::pxlcs_t R = static_cast<Pixel::pxlcs_t>((RGB & 0xff0000) >> 16);
			Pixel::pxlcs_t G = static_cast<Pixel::pxlcs_t>((RGB & 0xff00) >> 8);
			Pixel::pxlcs_t B = static_cast<Pixel::pxlcs_t>(RGB & 0xff);*/

			//Red - Green - Blue
			m_data[(height - row - 1) * width + col] = RGB;
		}

		ifile.seekg(padding, std::ios::cur);
	}

	//Set dimensions
	m_width = static_cast<Point::dim_t>(width);
	m_height = static_cast<Point::dim_t>(height);

	if (ifile.fail()) {
		std::cout << "The image was not loaded correctly!\n";
		clearAndClose();
		return false;
	}

	clearAndClose();
	return true;
}

bool Image::saveImage(const std::string &path) const {
	std::ofstream ofile(path, std::ios::binary | std::ios::trunc);
	if (!ofile) {
		std::cout << "Could not load the output file!\n";
		return false;
	}

	//Header of the .bmp file
	unsigned short fileHeader[] = {
		0x4D42,			//"BM", 2 bytes
		0x0000, 0x0000,	//file size, 4 bytes
		0x0000,			//reserverd, 2 bytes
		0x0000,			//reserved, 2 bytes
		0x0036, 0x0000	//start of array data, 4 bytes
	};

	//Header with the image information
	unsigned informationHeader[]{
		0x00000028,				//BITMAPCOREHEADER, 4 bytes
		(unsigned)m_width,		//width, 4 bytes
		(unsigned)m_height,		//height, 4 bytes
		0x00180001,				//1 plane, 2 bytes ; 24 bits per pixel, 2 bytes
		0x00000000,				//compression - 0, 4 bytes
		0x00000000,				//raw bitmap data, 4 bytes
		0x00000000,				//pixels per point - horizontal, 4 bytes 
		0x00000000,				//pixels per point - vertical, 4 bytes 
		0x00000000,				//number of colors in the palette, 4 bytes
		0x00000000				//0 means all colors are important, 4 bytes
	};

	//Save the headers
	ofile.write((const char *)&fileHeader, 14);			//fileHeader - 14 bytes
	ofile.write((const char *)&informationHeader, 40);	//informationHeader - 40 bytes

	//BMP is aligned at 4 bytes
	size_t padding = (4 - (m_width * 3) % 4) % 4;
	const size_t buffer = 0;

	for (Point::dim_t row = 0; row < m_height; ++row) {
		for (Point::dim_t col = 0; col < m_width; ++col) {
			const auto &pixel = m_data[(m_height - row - 1) * m_width + col];
			//uint32_t RGB = pixel.R() << 16u | pixel.G() << 8u | pixel.B();
			ofile.write((const char *)&pixel, 3);
		}

		ofile.write((const char *)&buffer, padding);
	}

	if (ofile.fail()) {
		std::cout << "The image might be not saved properly!\n";
	}

	bool output = ofile.good();
	ofile.clear();
	ofile.close();

	return output;
}

size_t Image::size() const {
	return m_data.size();
}

const std::string& Image::path() const {
	return m_imagePath;
}

Point::dim_t Image::width() const {
	return m_width;
}

Point::dim_t Image::height() const {
	return m_height;
}

const Pixel::pxl_t& Image::pixel(const Point &position) const {
	if (static_cast<size_t>(position.x() * m_width + position.y()) >= m_data.size()) {
		throw std::range_error("Out of bounds!");
	}

	return m_data[position.x() * m_width + position.y()];
}

void Image::setPixel(const Point &position, const Pixel::pxl_t &pxl) {
	if (static_cast<size_t>(position.x() * m_width + position.y()) >= m_data.size()) {
		throw std::range_error("Out of bounds!");
	}

	m_data[position.x() * m_width + position.y()] = pxl;
}
