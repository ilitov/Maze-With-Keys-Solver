#include <iostream>
#include "Image.h"
#include "ImageInfo.h"
#include "PathFinder.h"

int main() {
	Image img("./images/example.bmp");
	if (!img.loadImage()) {
		std::cout << "The image was not loaded properly!\n";
		return 1;
	}

	ImageInfo imgInfo(img);
	PathFinder finder(imgInfo);

	try {
		imgInfo.analyzeImage();
		
		if (finder.findPath()) {
			finder.drawPath();
			imgInfo.saveImage();
		}
		else {
			std::cout << "There is no path!\n";
		}

		finder.savePathPoints();
	}
	catch (std::exception &x) {
		std::cout << x.what() << '\n';
		return 1;
	}
	
	return 0;
}
