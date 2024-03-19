#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

//first
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	for (float t = 0.; t < 1.; t += .01) {
//		int x = x0 + (x1 - x0) * t;
//		int y = y0 + (y1 - y0) * t;
//		image.set(x, y, color);
//	}
//}

//second
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	for (int x = x0; x <= x1; x++) {
//		float t = (x - x0) / (float)(x1 - x0);
//		int y = y0 * (1. - t) + y1 * t;
//		image.set(x, y, color);
//	}
//}

//third
//selfCode:
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//	int dx = x1 - x0;
//	int dy = y1 - y0;
//	int xStart, xEnd, yStart, yEnd;
//	if (dx >= 0) {
//		xStart = x0;
//		xEnd = x1;
//	}
//	else {
//		xStart = x1;
//		xEnd = x0;
//	}
//	if (dy >= 0) {
//		yStart = y0;
//		yEnd = y1;
//	}
//	else {
//		yStart = y1;
//		yEnd = y0;
//	}
//	if (std::abs(dx) < std::abs(dy)) {
//		for (int y = yStart; y <= yEnd; y++) {
//			float t = (y - yStart) / (float)(yEnd - yStart);
//			int x = xStart * (1 - t) + t * xEnd;
//			image.set(x, y, color);
//		}
//	}
//	else {
//		for (int x = xStart; x <= xEnd; x++) {
//			float t = (x - xStart) / (float)(xEnd - xStart);
//			int y = yStart * (1 - t) + t * yEnd;
//			image.set(x, y, color);
//		}
//	}
//}
//courseCode
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//    bool steep = false;
//    if (std::abs(x0 - x1) < std::abs(y0 - y1)) { // if the line is steep, we transpose the image 
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        steep = true;
//    }
//    if (x0 > x1) { // make it left−to−right 
//        std::swap(x0, x1);
//        std::swap(y0, y1);
//    }
//    for (int x = x0; x <= x1; x++) {
//        float t = (x - x0) / (float)(x1 - x0);
//        int y = y0 * (1. - t) + y1 * t;
//        if (steep) {
//            image.set(y, x, color); // if transposed, de−transpose 
//        }
//        else {
//            image.set(x, y, color);
//        }
//    }
//}

//fourth
//void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
//    bool steep = false;
//    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        steep = true;
//    }
//    if (x0 > x1) {
//        std::swap(x0, x1);
//        std::swap(y0, y1);
//    }
//    int dx = x1 - x0;
//    int dy = y1 - y0;
//    float derror = std::abs(dy / float(dx));
//    float error = 0;
//    int y = y0;
//    for (int x = x0; x <= x1; x++) {
//        if (steep) {
//            image.set(y, x, color);
//        }
//        else {
//            image.set(x, y, color);
//        }
//        error += derror;
//        if (error > .5) {
//            y += (y1 > y0 ? 1 : -1);
//            error -= 1.;
//        }
//    }
//}

//fifth
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}


int main(int argc, char** argv) {
	/*TGAImage image(100, 100, TGAImage::RGB);
	image.set(52, 41, red);*/

	//first
	//line(2, 52, 90, 80, image, white);
	
	//second
	/*line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);*/

	//third
	/*line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);*/

    TGAImage image(800, 800, TGAImage::RGB);
    Model* model = new Model("obj/Body #395857.obj");
    float maxNum = model->getMaxNum();
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x / maxNum + 1.) * 800 / 2.;
            int y0 = (v0.y / maxNum + 1.) * 800 / 2.;
            int x1 = (v1.x / maxNum + 1.) * 800 / 2.;
            int y1 = (v1.y / maxNum + 1.) * 800 / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

