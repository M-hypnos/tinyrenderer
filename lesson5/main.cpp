#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);
const TGAColor pink = TGAColor(255, 0, 255, 255);

float M_PI = 3.14;

int Width = 800;
int Height = 800;
int depth = 255;
std::vector<float> zBuffer(Width * Height, -std::numeric_limits<float>::infinity());

Model* model = nullptr;
Vec3f light_dir = Vec3f(1, -1, 1).normalize();
Vec3f camera(1, 1, 3);
Vec3f target(0, 0, 0);

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

void line(Vec3i v0, Vec3i v1, TGAImage& image, TGAColor color) {
    line(v0.x, v0.y, v1.x, v1.y, image, color);
}

std::pair<float, float> getBarycentricCoordinates(Vec3i* pts, float x, float y) {
    float gamma = ((pts[0].y - pts[1].y) * x + (pts[1].x - pts[0].x) * y + pts[0].x * pts[1].y - pts[1].x * pts[0].y)
        / ((pts[0].y - pts[1].y) * pts[2].x + (pts[1].x - pts[0].x) * pts[2].y + pts[0].x * pts[1].y - pts[1].x * pts[0].y);
    float beta = ((pts[0].y - pts[2].y) * x + (pts[2].x - pts[0].x) * y + pts[0].x * pts[2].y - pts[2].x * pts[0].y)
        / ((pts[0].y - pts[2].y) * pts[1].x + (pts[2].x - pts[0].x) * pts[1].y + pts[0].x * pts[2].y - pts[2].x * pts[0].y);
    return {beta,gamma };
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Matrix viewport(int x, int y, int width, int height) {
    Matrix m = Matrix::identity(4);
    m[0][0] = width / 2.f;
    m[1][1] = height / 2.f;
    m[2][2] = depth / 2.f;
    m[0][3] = width / 2.f + x;
    m[1][3] = height / 2.f + y;
    m[2][3] = -depth / 2.f;
    return m;
}

Matrix viewMatrix(Vec3f cameraPos, Vec3f target, Vec3f up) {
    Vec3f z = (cameraPos - target).normalize();
    Vec3f x = (up ^ z).normalize();
    Vec3f y = (z ^ x).normalize();

    Matrix t = Matrix::identity(4);
    t[0][3] = -target.x;
    t[1][3] = -target.y;
    t[2][3] = -target.z;

    Matrix t1 = Matrix::identity(4);
    /*for (int i = 0; i < 3; i++) {
        t1[i][0] = x[i];
        t1[i][1] = y[i];
        t1[i][2] = z[i];
    }

    return t1.inverse() * t;*/
    for (int i = 0; i < 3; i++) {
        t1[0][i] = x[i];
        t1[1][i] = y[i];
        t1[2][i] = z[i];
    }
    return t1 * t;
}

Matrix scaleBy(float scale) {
    Matrix m = Matrix::identity(4);
    m[0][0] = m[1][1] = m[2][2] = scale;
    return m;
}

Matrix shearX(float value) {
    Matrix m = Matrix::identity(4);
    m[0][1] = value;
    return m;
}

Matrix shearY(float value) {
    Matrix m = Matrix::identity(4);
    m[1][0] = value;
    return m;
}

Matrix rotateZ(float angle) {
    float cosAngle = cos(angle * M_PI / 180.f);
    float sinAngle = sin(angle * M_PI / 180.f);
    Matrix m = Matrix::identity(4);
    m[0][0] = m[1][1] = cosAngle;
    m[0][1] = -sinAngle;
    m[1][0] = sinAngle;
    return m;
}

void triangle(Vec3f* worldcoords, Vec3i* screencoords, Vec3f* texcoords, Vec3f* normalcoords, TGAImage& image) {
    int minX = Width * 7 / 8 - 1;
    int minY = Height * 7 / 8 - 1;
    int maxX = Width / 8;
    int maxY = Height / 8;
    for (int i = 0; i < 3; i++) {
        minX = std::min(minX, screencoords[i].x);
        maxX = std::max(maxX, screencoords[i].x);
        minY = std::min(minY, screencoords[i].y);
        maxY = std::max(maxY, screencoords[i].y);
    }

    /*minX = std::max(Width / 8, minX);
    maxX = std::min(maxX, Width * 7 / 8 - 1);
    minY = std::max(Height / 8, minY);
    maxY = std::min(maxY, Height * 7 / 8 - 1);*/
    auto I1 = normalcoords[0] * light_dir;
    auto I2 = normalcoords[1] * light_dir;
    auto I3 = normalcoords[2] * light_dir;

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            //if (x > 600 && y > 500) x += 0.01;
            auto data = getBarycentricCoordinates(screencoords, x, y);
            auto beta = data.first;
            auto gamma = data.second;
            auto alpha = 1 - beta - gamma;
            if (alpha < -0.001 || beta < -0.001 || gamma < -0.001) continue;
            auto z = alpha * screencoords[0].z + beta * screencoords[1].z + gamma * screencoords[2].z;
            int idx = int(x + y * Width);
            if (zBuffer[idx] < z) {
                zBuffer[idx] = z;

                /*auto uv = texcoords[0] * alpha + texcoords[1] * beta + texcoords[2] * gamma;
                auto c = model->getDiffuseColor(uv.x, uv.y);

                image.set(x, y, TGAColor(-I * c.r, -I * c.g, -I * c.b));*/

                auto c = TGAColor(255, 255, 255) * I1 * alpha + TGAColor(255, 255, 255) * I2 * beta + TGAColor(255, 255, 255) * I3 * gamma;
                
                image.set(x, y, c);

            }
        }
    }
}

int main(int argc, char** argv) {
    TGAImage image(Width, Height, TGAImage::RGB);
    model = new Model("obj/african_head.obj");
    model->loadDiffuseTexture("obj/african_head_diffuse.tga");
    Matrix vpm = viewport(Width / 8, Height / 8, Width * 3 / 4, Height * 3 / 4);

    /*Vec3f x(1.f, 0.f, 0.f), y(0.f, 1.f, 0.f), o(0.f, 0.f, 0.f);
    o = m2v(vpm * v2m(o));
    x = m2v(vpm * v2m(x));
    y = m2v(vpm * v2m(y));
    line(y, o, image, red);
    line(o, x, image, red);*/

    //for (int i = 0; i < model->nfaces(); i++) {
    //    std::vector<int> face = model->face(i);
    //    for (int j = 0; j < face.size(); j++) {
    //        Vec3f v0 = model->vert(face[j]);
    //        Vec3f v1 = model->vert(face[(j+1) % face.size()]);
    //        /*{
    //            Vec3f vp0 = m2v(vpm * v2m(v0));
    //            Vec3f vp1 = m2v(vpm * v2m(v1));
    //            line(vp0, vp1, image, white);
    //        }
    //        {
    //            Matrix translate = shearX(-1/3.f);
    //            Vec3f vp0 = m2v(vpm * translate * v2m(v0));
    //            Vec3f vp1 = m2v(vpm * translate * v2m(v1));
    //            line(vp0, vp1, image, red);

    //            translate = shearY(1 / 3.f) * translate;
    //            Vec3f vq0 = m2v(vpm * translate * v2m(v0));
    //            Vec3f vq1 = m2v(vpm * translate * v2m(v1));
    //            line(vq0, vq1, image, green);
    //        }
    //        {
    //            Matrix translate = rotateZ(30);
    //            Vec3f vp0 = m2v(vpm * translate * v2m(v0));
    //            Vec3f vp1 = m2v(vpm * translate * v2m(v1));
    //            line(vp0, vp1, image, yellow);
    //        }*/
    //        {
    //            Matrix translate = Matrix::identity(4);
    //            translate[3][0] = -0.2f;
    //            Vec3f vp0 = m2v(vpm * translate * v2m(v0));
    //            Vec3f vp1 = m2v(vpm * translate * v2m(v1));
    //            line(vp0, vp1, image, yellow);
    //        }
    //    }
    //    break;
    //}

    Matrix projection = Matrix::identity(4);
    projection[3][2] = -1.f / (camera - target).norm();
    Matrix view = viewMatrix(camera, target, Vec3f(0, 1, 0));

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f worldcoord[3];
        std::vector<int> tface = model->tface(i);
        std::vector<int> nface = model->noface(i);
        Vec3f texcoord[3];
        Vec3i screencoords[3];
        Vec3f normalcoords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            worldcoord[j] = v0;
            screencoords[j] = m2v(vpm * projection * view * v2m(v0));
            Vec3f v1 = model->tverts(tface[j]);
            texcoord[j] = v1;
            Vec3f v2 = model->nverts(nface[j]);
            normalcoords[j] = v2;
        }
        //Vec3f n = (worldcoord[1] - worldcoord[0]) ^ (worldcoord[2] - worldcoord[0]);
        //n.normalize();
        //float I = n * light_dir;
        //if (I < 0) {
            triangle(worldcoord, screencoords, texcoord, normalcoords, image);
        //}
    }

    //{ // dump z-buffer (debugging purposes only)
    //    TGAImage zbimage(Width, Height, TGAImage::GRAYSCALE);
    //    for (int i = 0; i < Width; i++) {
    //        for (int j = 0; j < Height; j++) {
    //            zbimage.set(i, j, TGAColor(zBuffer[i + j * Width], 1));
    //        }
    //    }
    //    zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    //    zbimage.write_tga_file("zbuffer.tga");
    //}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

