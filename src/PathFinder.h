#pragma once
#ifndef PATH_FINDER_CLASS_HEADER
#define PATH_FINDER_CLASS_HEADER

#include <vector>
#include "ImageInfo.h"

class PathFinder {
public:
	PathFinder();
	PathFinder(ImageInfo &imageInfo);
	PathFinder(const PathFinder &r) = default;
	PathFinder& operator=(const PathFinder &rhs) = default;
	~PathFinder() = default;

public:
	bool findPath();
	void drawPath();
	void savePathPoints();

	// Updates the pointer to the object of type 'ImageInfo'
	void setImageData(ImageInfo &imageInfo);

private:
	size_t closestKeyCost(const Point &to) const;
	bool intersectKey(const Point &point) const;
	bool hasKey(const Pixel::pxl_t &pixel) const;
	bool walkable(Point::dim_t x, Point::dim_t y) const;
	void addNewPath(const Point &src, const std::vector<Point> &came_from);

	const std::vector<Point> neighbours(const Point &point) const;
	const std::vector<Point> successors(const Point &curr, const Point &parent, Point &impJumpPoint);
	const Point jump(const Point &curr, const Point &next, Point &impJumpPoint);

	bool forcedNeigbour(const Point &walk, const Point &neighbour) const;
	static size_t manhattanDist(const Point &a, const Point &b);

	//	Methods related to line drawing
	static float linearInterpolation(Point::dim_t start, Point::dim_t end, float t);
	static const Point linInterpPoint(const Point &from, const Point &to, float t);
	static Point::dim_t diagonalDistance(const Point &a, const Point &b);
	static const Point roundPoint(float x, float y);
	static const std::vector<Point> line(const Point &from, const Point &to);

private:
	ImageInfo *m_imgData;
	std::vector<Pixel::pxl_t> m_inventory;		//	Inventory of collected keys
	std::vector<std::vector<Point>> m_paths;	//	Collection of path segments
};

#endif // !PATH_FINDER_CLASS_HEADER
