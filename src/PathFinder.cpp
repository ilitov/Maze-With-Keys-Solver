#include <iostream>
#include <limits>
#include <cmath>
#include <queue>
#include "PathFinder.h"

PathFinder::PathFinder()
	: m_imgData(nullptr) {

}

PathFinder::PathFinder(ImageInfo &imageInfo)
	: PathFinder() {
	setImageData(imageInfo);
}

bool PathFinder::findPath() {
	//	Check for null pointer(possible problems if the pointer is dangling)
	if (!m_imgData) {
		std::cout << "There is no image to process!\n";
		return false;
	}
	//	Valid pointer but invalid labyrinth
	else if (m_imgData->getEnds().empty() || m_imgData->startPoint() == Point{ -1,-1 }) {
		return false;
	}

	//	Clear all accumulated information
	m_inventory.clear();
	m_paths.clear();

	//	Used for the priority queue
	using qType = std::pair<Point, size_t>;
	struct Comp { bool operator()(const qType &lhs, const qType &rhs) const { return lhs.second > rhs.second; } };

	const size_t imgWidth = m_imgData->getImage().width();
	const size_t imgSize = m_imgData->getImage().size();

	//	The algorithm uses several starting points for its inner search
	std::queue<Point> startingPoints;
	startingPoints.push(m_imgData->startPoint());

	bool endFound = false;

	//	Vector of parents for every pixel
	std::vector<Point> came_from(imgSize);

	//	Cost/distance for every pixel
	std::vector<size_t> cost(imgSize);

	//	Iterate over the starting points
	while (!endFound && !startingPoints.empty()) {
		//	Clear the information for the parents from the previous search
		came_from.assign(imgSize, Point{ -1,-1 });
		came_from[startingPoints.front().x() * imgWidth + startingPoints.front().y()] = startingPoints.front();

		//	Reset the distances
		cost.assign(imgSize, std::numeric_limits<size_t>::max());
		cost[startingPoints.front().x() * imgWidth + startingPoints.front().y()] = 0;

		std::priority_queue<qType, std::vector<qType>, Comp> currQueue;
		currQueue.push(std::make_pair(startingPoints.front(), 0));
		startingPoints.pop();

		//	Used for Jump Point Search(JPS) Algorithm
		Point importantJumpPoint{ -1,-1 };

		//	Inner search loop
		while (!currQueue.empty()) {
			const auto currPoint = currQueue.top().first;
			currQueue.pop();

			//	Returns a vector of points to be explored
			const auto &frontier = successors(currPoint, came_from[currPoint.x() * imgWidth + currPoint.y()], importantJumpPoint);
			for (const auto &point : frontier) {

				size_t newCost = cost[currPoint.x() * imgWidth + currPoint.y()] + manhattanDist(currPoint, point);

				if (newCost < cost[point.x() * imgWidth + point.y()]) {
					came_from[point.x() * imgWidth + point.y()] = currPoint;
					cost[point.x() * imgWidth + point.y()] = newCost;

					size_t priority = newCost + closestKeyCost(point);
					currQueue.push(std::make_pair(point, priority));
				}
			}

			//	Found a new key or an ending zone
			//	We stop and 'jump' from that point
			if (importantJumpPoint != Point{ -1,-1 }) {

				//	Update the startingPoint
				if (m_imgData->getImage().pixel(importantJumpPoint) != Pixel::END) {
					startingPoints.push(importantJumpPoint);
				}
				else {
					endFound = true;
				}

				addNewPath(frontier.back(), came_from);
				break;
			}
		}
	}

	//	Clear any saved data if there is no solution
	if (!endFound) {
		m_inventory.clear();
		m_paths.clear();
	}

	return endFound;
}

void PathFinder::drawPath() {
	if (m_paths.empty() || !m_imgData) {
		std::cout << "There are no paths to draw!\n";
		return;
	}

	auto &image = m_imgData->getImage();

	for (const auto &currPath : m_paths) {
		for (size_t i = 0; i + 1 < currPath.size(); ++i) {
			const auto &linePoints = line(currPath[i], currPath[i + 1]);

			for (const auto &point : linePoints) {
				image.setPixel(point, Pixel::DRAW);
			}
		}
	}
}

void PathFinder::savePathPoints() {
	if (!m_imgData) {
		std::cout << "There is no data from processed image!\n";
		return;
	}

	std::string ouputFileName = m_imgData->getImage().path();
	while (ouputFileName.back() != '.') {
		ouputFileName.pop_back();
	}

	ouputFileName.pop_back();
	ouputFileName.append("_outputPoints.txt");

	std::ofstream ofile(ouputFileName);
	if (!ofile) {
		std::cout << "The file with path points cannot be generated!\n";
		return;
	}

	if (m_paths.empty()) {
		ofile << "No solution!\n";
	}
	else {
		for (const auto &currPath : m_paths) {
			for (size_t i = currPath.size() - 1; i > 0; --i) {
				ofile << currPath[i] << "\n";
			}
		}

		ofile << m_paths.back().front() << "\n";
	}

	ofile.clear();
	ofile.close();
}

void PathFinder::setImageData(ImageInfo &imageInfo) {
	m_imgData = &imageInfo;

	m_inventory.clear();
	m_paths.clear();
}

size_t PathFinder::closestKeyCost(const Point &to) const {
	const auto &keys = m_imgData->getKeys();
	const auto &ends = m_imgData->getEnds();
	size_t minDist = UINT32_MAX;

	//	Iterate over the keys
	for (const auto &key : keys) {
		//	If the key is already in the inventory we do not need it
		if (hasKey(key.first)) {
			continue;
		}

		for (const auto &point : key.second) {
			size_t currDist = manhattanDist(point, to);
			minDist = std::min(minDist, currDist);
		}
	}

	//	Iterate over the ending zones
	for (const auto &end : ends) {
		size_t currDist = manhattanDist(end, to);
		minDist = std::min(minDist, currDist);
	}

	return minDist;
}

bool PathFinder::intersectKey(const Point &point) const {
	const auto &keys = m_imgData->getKeys();
	const auto &pxl = m_imgData->getImage().pixel(point);

	auto iterPxlVal = keys.find(pxl);
	if (iterPxlVal == keys.end()) {
		return false;
	}

	for (const auto &position : iterPxlVal->second) {
		if (manhattanDist(point, position) <= static_cast<size_t>(ImageInfo::KEY_WIDTH)) {
			return true;
		}
	}

	return false;
}

bool PathFinder::hasKey(const Pixel::pxl_t &pixel) const {
	for (const auto &key : m_inventory) {
		if (key == pixel) {
			return true;
		}
	}

	return false;
}

bool PathFinder::walkable(Point::dim_t x, Point::dim_t y) const {
	return x >= 0 && x < m_imgData->getImage().height() && y >= 0 && y < m_imgData->getImage().width() && m_imgData->getImage().pixel(Point{ x, y }) != Pixel::BLACK;
}

void PathFinder::addNewPath(const Point &src, const std::vector<Point> &came_from) {
	size_t width = m_imgData->getImage().width();
	Point tmp{ src };

	if (tmp != m_paths.back().back()) {
		m_paths.back().push_back(tmp);
	}

	Point::dim_t last_dx = 0;
	Point::dim_t last_dy = 0;

	Point::dim_t dx = 0;
	Point::dim_t dy = 0;

	while (tmp != came_from[tmp.x() * width + tmp.y()]) {
		tmp = came_from[tmp.x() * width + tmp.y()];

		dx = tmp.x() - m_paths.back().back().x();
		dy = tmp.y() - m_paths.back().back().y();

		if (dx != last_dx || dy != last_dy) {
			m_paths.back().push_back(tmp);
		}

		last_dx = dx;
		last_dy = dy;
	}
}

const std::vector<Point> PathFinder::neighbours(const Point &point) const {
	const size_t nSize = 8;
	const Point::dim_t dir[nSize][2] = { {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0},{1,1}, {0,1},{-1,1} };

	std::vector<Point> result;

	for (size_t i = 0; i < nSize; ++i) {
		Point::dim_t nextX = point.x() + dir[i][0];
		Point::dim_t nextY = point.y() + dir[i][1];

		if (walkable(nextX, nextY)) {
			result.push_back(Point{ nextX, nextY });
		}
	}

	return result;
}

const std::vector<Point> PathFinder::successors(const Point &curr, const Point &parent, Point &impJumpPoint) {
	std::vector<Point> successors;
	std::vector<Point> neigh;

	Point::dim_t dx = curr.x() - parent.x();
	Point::dim_t dy = curr.y() - parent.y();

	if (dx != 0) {
		dx = dx > 0 ? 1 : -1;
	}
	if (dy != 0) {
		dy = dy > 0 ? 1 : -1;
	}

	/* Prune neighbours */

	//	Starting point, there is no direction -> jump in all 8 possible directions
	if (dx == 0 && dy == 0) {
		neigh = neighbours(curr);
	}
	//	Diagonal movement
	else if (dx != 0 && dy != 0) {
		neigh.push_back(Point{ curr.x() + dx, curr.y() });
		neigh.push_back(Point{ curr.x(), curr.y() + dy });
		neigh.push_back(Point{ curr.x() + dx, curr.y() + dy });

		//	Check for forced neighbours
		if (forcedNeigbour(Point{ curr.x() - dx, curr.y() + dy }, Point{ curr.x() - dx, curr.y() })) {
			neigh.push_back(Point{ curr.x() - dx, curr.y() + dy });
		}

		if (forcedNeigbour(Point{ curr.x() + dx, curr.y() - dy }, Point{ curr.x(), curr.y() - dy })) {
			neigh.push_back(Point{ curr.x() + dx, curr.y() - dy });
		}
	}
	//	Vertical or horizontal movement
	else {
		neigh.push_back(Point{ curr.x() + dx, curr.y() + dy });

		//	Check for forced neighbours
		if (dx != 0) {
			//	Vertical movement
			if (forcedNeigbour(Point{ curr.x() + dx, curr.y() + 1 }, Point{ curr.x(), curr.y() + 1 })) {
				neigh.push_back(Point{ curr.x() + dx, curr.y() + 1 });
			}

			if (forcedNeigbour(Point{ curr.x() + dx, curr.y() - 1 }, Point{ curr.x(), curr.y() - 1 })) {
				neigh.push_back(Point{ curr.x() + dx, curr.y() - 1 });
			}
		}
		else {
			//	Horizontal movement
			if (forcedNeigbour(Point{ curr.x() + 1, curr.y() + dy }, Point{ curr.x() + 1, curr.y() })) {
				neigh.push_back(Point{ curr.x() + 1, curr.y() + dy });
			}

			if (forcedNeigbour(Point{ curr.x() - 1, curr.y() + dy }, Point{ curr.x() - 1, curr.y() })) {
				neigh.push_back(Point{ curr.x() - 1, curr.y() + dy });
			}
		}
	}

	//	Iterate over all directions in which we have to 'jump'
	for (size_t i = 0; i < neigh.size() && impJumpPoint.x() == -1 && impJumpPoint.y() == -1; ++i) {
		const auto &jumpPoint = jump(curr, neigh[i], impJumpPoint);

		if (jumpPoint.x() != -1 && jumpPoint.y() != -1) {
			successors.push_back(jumpPoint);
		}
	}

	return successors;
}

const Point PathFinder::jump(const Point &curr, const Point &next, Point &impJumpPoint) {
	Point::dim_t currX = curr.x();
	Point::dim_t currY = curr.y();

	Point::dim_t x = next.x();
	Point::dim_t y = next.y();

	//Keep the value of the last pixel
	auto lastPixel = m_imgData->getImage().pixel(Point{ currX, currY });

	while (true) {
		Point::dim_t dx = x - currX;
		Point::dim_t dy = y - currY;

		if (!walkable(x, y)) {
			return Point{ -1,-1 };
		}

		auto currPixel = m_imgData->getImage().pixel(Point{ x, y });

		//	Colored zone
		if (ImageInfo::color(currPixel) && currPixel != lastPixel) {
			bool canUnlock = hasKey(currPixel);
			bool key = !canUnlock && intersectKey(Point{ x, y });

			//	Collect the key
			if (key) {
				m_inventory.push_back(currPixel);
				impJumpPoint = Point{ x, y };

				m_paths.push_back(std::vector<Point>());
				m_paths.back().push_back(impJumpPoint);

				return Point{ x, y };
			}
			//	Not walkable because there is no such key in the inventory
			else if (!key && !canUnlock) {
				return Point{ -1,-1 };
			}
		}
		//	Check for ending zone
		else if (currPixel == Pixel::END) {
			impJumpPoint = Point{ x, y };

			m_paths.push_back(std::vector<Point>());
			m_paths.back().push_back(Point{ x, y });

			return Point{ x, y };
		}

		//	Diagonal movement
		if (dx != 0 && dy != 0) {
			//	Forced neighbours
			if (forcedNeigbour(Point{ x - dx, y + dy }, Point{ x - dx, y }) ||
				forcedNeigbour(Point{ x + dx, y - dy }, Point{ x, y - dy })) {

				return Point{ x, y };
			}

			//	Vertical and horizontal jumps
			if (jump(Point{ x, y }, Point(x + dx, y), impJumpPoint) != Point{ -1,-1 } ||
				jump(Point{ x, y }, Point(x, y + dy), impJumpPoint) != Point{ -1,-1 }) {

				return Point{ x, y };
			}
		}
		//	Vertical and horizontal movement
		else {
			//	Forced neighbours check
			if (dx != 0) {
				//	Vertical movement
				if (forcedNeigbour(Point{ x + dx, y + 1 }, Point{ x, y + 1 }) ||
					forcedNeigbour(Point{ x + dx, y - 1 }, Point{ x, y - 1 })) {

					return Point{ x, y };
				}
			}
			else {
				//	Horizontal movement
				if (forcedNeigbour(Point{ x + 1, y + dy }, Point{ x + 1, y }) ||
					forcedNeigbour(Point{ x - 1, y + dy }, Point{ x - 1, y })) {

					return Point{ x, y };
				}
			}
		}

		//	Update the value of the previous pixel
		lastPixel = currPixel;

		currX = x;
		currY = y;

		x += dx;
		y += dy;
	}
}

bool PathFinder::forcedNeigbour(const Point &walk, const Point &neighbour) const {
	if (walkable(walk.x(), walk.y())) {
		const auto &neighbourPixel = m_imgData->getImage().pixel(neighbour);

		if (neighbourPixel == Pixel::BLACK || ImageInfo::color(neighbourPixel) && !hasKey(neighbourPixel)) {
			return true;
		}
	}

	return false;
}

size_t PathFinder::manhattanDist(const Point &a, const Point &b) {
	return std::abs(a.x() - b.x()) + std::abs(a.y() - b.y());
}

float PathFinder::linearInterpolation(Point::dim_t start, Point::dim_t end, float t) {
	return static_cast<float>(start) + t * (end - start);
}

const Point PathFinder::linInterpPoint(const Point &from, const Point &to, float t) {
	return roundPoint(linearInterpolation(from.x(), to.x(), t), linearInterpolation(from.y(), to.y(), t));
}

Point::dim_t PathFinder::diagonalDistance(const Point &a, const Point &b) {
	return std::max(std::abs(a.x() - b.x()), std::abs(a.y() - b.y()));
}

const Point PathFinder::roundPoint(float x, float y) {
	return Point{ static_cast<Point::dim_t>(std::round(x)), static_cast<Point::dim_t>(std::round(y)) };
}

const std::vector<Point> PathFinder::line(const Point &from, const Point &to) {
	Point::dim_t diagDistance = diagonalDistance(from, to);
	
	std::vector<Point> points;
	points.reserve(diagDistance + 1);

	for (Point::dim_t step = 0; step <= diagDistance; ++step) {
		float t = 0.0f;
		if (diagDistance > 0) {
			t = step / static_cast<float>(diagDistance);
		}

		points.push_back(linInterpPoint(from, to, t));
	}

	return points;
}
