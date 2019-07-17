#pragma once
#ifndef PIXEL_CLASS_HEADER
#define PIXEL_CLASS_HEADER

#include <cstdint>		//uint8_t

class Pixel {
public:
	using pxl_t = size_t;		//size of each pixel
	using pxlcs_t = uint8_t;	//size of each component of a pixel

public:
	static const pxl_t BLACK;
	static const pxl_t WHITE;
	static const pxl_t START;
	static const pxl_t END;
	static const pxl_t DRAW;

public:
	explicit Pixel(pxlcs_t A, pxlcs_t R, pxlcs_t G, pxlcs_t B);
	Pixel(pxlcs_t R = 0, pxlcs_t G = 0, pxlcs_t B = 0);
	Pixel(const Pixel &r) = default;
	Pixel& operator=(const Pixel &rhs) = default;
	~Pixel() = default;

public:
	pxlcs_t A() const;
	pxlcs_t R() const;
	pxlcs_t G() const;
	pxlcs_t B() const;

	void setA(pxlcs_t A);
	void setR(pxlcs_t R);
	void setG(pxlcs_t G);
	void setB(pxlcs_t B);

private:
	pxlcs_t m_R;
	pxlcs_t m_G;
	pxlcs_t m_B;
	pxlcs_t m_A;
};

//	The three operators below are not used but may be needed
//	if we use specific containers of Pixels
bool operator==(const Pixel &lhs, const Pixel &rhs);
bool operator!=(const Pixel &lhs, const Pixel &rhs);
bool operator<(const Pixel &lhs, const Pixel &rhs);

#endif // !PIXEL_CLASS_HEADER
