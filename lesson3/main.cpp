#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255,   0,   255);

int Width = 800;
int Height = 800;
std::vector<float> zBuffer(Width * Height, -std::numeric_limits<float>::infinity());

//lesson1 ------------------------------------------------------------
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
//    int derror2 = std::abs(dy) * 2;
//    int error2 = 0;
//    int y = y0;
//    for (int x = x0; x <= x1; x++) {
//        if (steep) {
//            image.set(y, x, color);
//        }
//        else {
//            image.set(x, y, color);
//        }
//        error2 += derror2;
//        if (error2 > dx) {
//            y += (y1 > y0 ? 1 : -1);
//            error2 -= dx * 2;
//        }
//    }
//}
//lesson1 ------------------------------------------------------------

//lesson2 ------------------------------------------------------------
//void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color, bool isSolid) {
//    if (isSolid) {
//        if (t0.y > t1.y) std::swap(t0, t1);
//        if (t0.y > t2.y) std::swap(t0, t2);
//        if (t1.y > t2.y) std::swap(t1, t2);
//        for (int y = t0.y; y < t1.y; y++) {
//            float ratioA = (y - t0.y) / (float)(t2.y - t0.y + 1);
//            float ratioB = (y - t0.y) / (float)(t1.y - t0.y + 1);
//            Vec2i tA = t0 + (t2 - t0) * ratioA;
//            Vec2i tB = t0 + (t1 - t0) * ratioB;
//            if (tA.x < tB.x) {
//                for (int x = tA.x; x <= tB.x; x++) {
//                    image.set(x, tA.y, color);
//                }
//            }
//            else {
//                for (int x = tB.x; x <= tA.x; x++) {
//                    image.set(x, tA.y, color);
//                }
//            }
//        }
//        for (int y = t1.y; y <= t2.y; y++) {
//            float ratioA = (y - t0.y) / (float)(t2.y - t0.y + 1);
//            float ratioB = (y - t1.y) / (float)(t2.y - t1.y + 1);
//            Vec2i tA = t0 + (t2 - t0) * ratioA;
//            Vec2i tB = t1 + (t2 - t1) * ratioB;
//            if (tA.x < tB.x) {
//                for (int x = tA.x; x <= tB.x; x++) {
//                    image.set(x, tA.y, color);
//                }
//            }
//            else {
//                for (int x = tB.x; x <= tA.x; x++) {
//                    image.set(x, tA.y, color);
//                }
//            }
//        }
//        image.set(t2.x, t2.y, color);
//    }
//    else {
//        line(t0.x, t0.y, t1.x, t1.y, image, color);
//        line(t1.x, t1.y, t2.x, t2.y, image, color);
//        line(t2.x, t2.y, t0.x, t0.y, image, color);
//    }
//}

//bool insideTriangle(Vec2i* pts, int x, int y) {
//    Vec2f a = Vec2f(pts[1].x - pts[0].x, pts[1].y - pts[0].y);
//    Vec2f b = Vec2f(pts[2].x - pts[1].x, pts[2].y - pts[1].y);
//    Vec2f c = Vec2f(pts[0].x - pts[2].x, pts[0].y - pts[2].y);
//    Vec2f pa = Vec2f(x - pts[0].x, y - pts[0].y);
//    Vec2f pb = Vec2f(x - pts[1].x, y - pts[1].y);
//    Vec2f pc = Vec2f(x - pts[2].x, y - pts[2].y);
//    bool flag = pa.x * a.y - pa.y * a.x <= 0;
//    if (flag != pb.x * b.y - pb.y * b.x <= 0) return false;
//    if (flag != pc.x * c.y - pc.y * c.x <= 0) return false;
//    return true;
//}
//
//void triangle(Vec2i* pts, TGAImage& image, TGAColor color) {
//    int minX = image.get_width() - 1;
//    int minY = image.get_height() - 1;
//    int maxX = 0;
//    int maxY = 0;
//    for (int i = 0; i < 3; i++) {
//        minX = std::min(minX, pts[i].x);
//        maxX = std::max(maxX, pts[i].x);
//        minY = std::min(minY, pts[i].y);
//        maxY = std::max(maxY, pts[i].y);
//    }
//    minX = std::max(0, minX);
//    maxX = std::min(maxX, image.get_width() - 1);
//    minY = std::max(0, minY);
//    maxY = std::min(maxY, image.get_height() - 1);
//
//    for (int x = minX; x <= maxX; x++) {
//        for (int y = minY; y <= maxY; y++) {
//            if (insideTriangle(pts, x, y)) {
//                image.set(x, y, color);
//            }
//        }
//    }
//}
//lesson2 ------------------------------------------------------------

//lesson3 ------------------------------------------------------------
std::pair<float, float> getBarycentricCoordinates(Vec3f* pts, float x, float y) {
    float beta = ((pts[0].y - pts[1].y) * x + (pts[1].x - pts[0].x) * y + pts[0].x * pts[1].y - pts[1].x * pts[0].y)
        / ((pts[0].y - pts[1].y) * pts[2].x + (pts[1].x - pts[0].x) * pts[2].y + pts[0].x * pts[1].y - pts[1].x * pts[0].y);
    float gamma = ((pts[0].y - pts[2].y) * x + (pts[2].x - pts[0].x) * y + pts[0].x * pts[2].y - pts[2].x * pts[0].y)
        / ((pts[0].y - pts[2].y) * pts[1].x + (pts[2].x - pts[0].x) * pts[1].y + pts[0].x * pts[2].y - pts[2].x * pts[0].y);
    return { beta,gamma };
}

void triangle(Vec3f* pts, TGAImage& image, TGAColor color) {
    float minX = image.get_width() - 1;
    float minY = image.get_height() - 1;
    float maxX = 0;
    float maxY = 0;
    for (int i = 0; i < 3; i++) {
        minX = std::min(minX, pts[i].x);
        maxX = std::max(maxX, pts[i].x);
        minY = std::min(minY, pts[i].y);
        maxY = std::max(maxY, pts[i].y);
    }
    minX = std::max(0.f, minX);
    maxX = std::min(maxX, (float)image.get_width() - 1);
    minY = std::max(0.f, minY);
    maxY = std::min(maxY, (float)image.get_height() - 1);

    for (float x = minX; x <= maxX; x++) {
        for (float y = minY; y <= maxY; y++) {
            auto data = getBarycentricCoordinates(pts, x, y);
            float alpha = 1 - data.first - data.second;
            float beta = data.first;
            float gamma = data.second;
            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                auto z = alpha * pts[0].z + beta * pts[1].z + gamma * pts[2].z;
                int idx = int(x) + int(y) * Width;
                if (zBuffer[idx] < z) {
                    zBuffer[idx] = z;
                    image.set(x, y, color);
                }
            }
        }
    }
}

//lesson3 ------------------------------------------------------------

int main(int argc, char** argv) {
    //lesson1 ------------------------------------------------------------
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

    /*TGAImage image(800, 800, TGAImage::RGB);
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
    }*/
    //lesson1 ------------------------------------------------------------

    //lesson2 ------------------------------------------------------------
    /*TGAImage image(200, 200, TGAImage::RGB);*/
    /*Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
    Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
    Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
    triangle(t0[0], t0[1], t0[2], image, red, false);
    triangle(t1[0], t1[1], t1[2], image, white, true);
    triangle(t2[0], t2[1], t2[2], image, green, true);*/

    /*Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
    triangle(pts, image, TGAColor(255, 0, 0, 255));*/
    //TGAImage image(800, 800, TGAImage::RGB);
    //Model* model = new Model("obj/Body #395857.obj");
    //float maxNum = model->getMaxNum();
    //Vec3f lightDir(0, 0, -1);
    //for (int i = 0; i < model->nfaces(); i++) {
    //    std::vector<int> face = model->face(i);
    //    Vec2i vp[3];
    //    Vec3f vf[3];
    //    for (int j = 0; j < 3; j++) {
    //        Vec3f v0 = model->vert(face[j]);
    //        vp[j] = Vec2i((v0.x / maxNum + 1.) * 800 / 2., (v0.y / maxNum + 1.) * 800 / 2.);
    //        vf[j] = v0;
    //    }
    //    //triangle(vp, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    //    Vec3f n = (vf[1] - vf[0]) ^ (vf[2] - vf[0]);
    //    n.normalize();
    //    float I =  n * lightDir;
    //    if (I < 0) {
    //        triangle(vp, image, TGAColor(-I * 255, -I * 255, -I * 255, 255));
    //    }
    //}
    //lesson2 ------------------------------------------------------------
    // 
    //lesson3 ------------------------------------------------------------
    TGAImage image(Width, Height, TGAImage::RGB);
    Model* model = new Model("obj/Body #395857.obj");
    float maxNum = model->getMaxNum();
    Vec3f lightDir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f vp[3];
        Vec3f vf[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            vp[j] = Vec3f(int((v0.x / maxNum + 1.) * 800 / 2. + .5), int((v0.y / maxNum + 1.) * 800 / 2. + .5), v0.z);
            vf[j] = v0;
        }
        Vec3f n = (vf[1] - vf[0]) ^ (vf[2] - vf[0]);
        n.normalize();
        float I = n * lightDir;
        if (I < 0) {
            triangle(vp, image, TGAColor(-I * 255, -I * 255, -I * 255, 255));
        }
    }
    //lesson3 ------------------------------------------------------------

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

