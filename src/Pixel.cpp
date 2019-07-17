#include "Pixel.h"

const Pixel::pxl_t Pixel::BLACK = 0x00000000u;	// Pixel(0, 0, 0)
const Pixel::pxl_t Pixel::WHITE = 0x00ffffffu;	// Pixel(255, 255, 255)
const Pixel::pxl_t Pixel::START = 0x00c3c3c3u;	// Pixel(195, 195, 195)
const Pixel::pxl_t Pixel::END	= 0x007f7f7fu;	// Pixel(127, 127, 127)
const Pixel::pxl_t Pixel::DRAW	= 0x00ff0000u;	// Pixel(255, 0, 0)

Pixel::Pixel(pxlcs_t A, pxlcs_t R, pxlcs_t G, pxlcs_t B)
	: m_R(R)
	, m_G(G)
	, m_B(B)
	, m_A(A) {

}

Pixel::Pixel(pxlcs_t R, pxlcs_t G, pxlcs_t B)
	: Pixel(255, R, G, B) {

}

Pixel::pxlcs_t Pixel::A() const {
	return m_A;
}

Pixel::pxlcs_t Pixel::R() const {
	return m_R;
}

Pixel::pxlcs_t Pixel::G() const {
	return m_G;
}

Pixel::pxlcs_t Pixel::B() const {
	return m_B;
}

void Pixel::setA(pxlcs_t A) {
	m_A = A;
}

void Pixel::setR(pxlcs_t R) {
	m_R = R;
}

void Pixel::setG(pxlcs_t G) {
	m_G = G;
}

void Pixel::setB(pxlcs_t B) {
	m_B = B;
}

bool operator==(const Pixel &lhs, const Pixel &rhs) {
	return	lhs.R() == rhs.R() && 
			lhs.G() == rhs.G() && 
			lhs.B() == rhs.B() && 
			lhs.A() == rhs.A();
}

bool operator!=(const Pixel &lhs, const Pixel &rhs) {
	return !(lhs == rhs);
}

bool operator<(const Pixel &lhs, const Pixel &rhs) {
	uint32_t rgbL = (lhs.R() << 16) | (lhs.G() << 8) | lhs.B();
	uint32_t rgbR = (rhs.R() << 16) | (rhs.G() << 8) | rhs.B();

	return rgbL < rgbR;
}
