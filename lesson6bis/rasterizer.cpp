#include "rasterizer.h"

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

std::pair<float, float> getBarycentricCoordinates(Vec3f* pts, float x, float y) {
    float gamma = ((pts[0].y - pts[1].y) * x + (pts[1].x - pts[0].x) * y + pts[0].x * pts[1].y - pts[1].x * pts[0].y)
        / ((pts[0].y - pts[1].y) * pts[2].x + (pts[1].x - pts[0].x) * pts[2].y + pts[0].x * pts[1].y - pts[1].x * pts[0].y);
    float beta = ((pts[0].y - pts[2].y) * x + (pts[2].x - pts[0].x) * y + pts[0].x * pts[2].y - pts[2].x * pts[0].y)
        / ((pts[0].y - pts[2].y) * pts[1].x + (pts[2].x - pts[0].x) * pts[1].y + pts[0].x * pts[2].y - pts[2].x * pts[0].y);
    return { beta,gamma };
}

void Rasterizer::triangle(Vec4f* pts, IShader& shader, TGAImage& image) {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = -std::numeric_limits<float>::max();
    float maxY = -std::numeric_limits<float>::max();
    for (int i = 0; i < 3; i++) {
        minX = std::min(minX, pts[i][0] / pts[i][3]);
        maxX = std::max(maxX, pts[i][0] / pts[i][3]);
        minY = std::min(minY, pts[i][1] / pts[i][3]);
        maxY = std::max(maxY, pts[i][1] / pts[i][3]);
    }

    maxX = std::min(maxX, width_ - 1.f);
    maxY = std::min(maxY, width_ - 1.f);
    minX = std::max(minX, 0.f);
    minY = std::max(minY, 0.f);

    Vec3f sv[3];
    sv[0] = proj<3>(pts[0] / pts[0][3]);
    sv[1] = proj<3>(pts[1] / pts[1][3]);
    sv[2] = proj<3>(pts[2] / pts[2][3]);
    TGAColor color;

    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            auto data = getBarycentricCoordinates(sv, x, y);
            auto alpha = 1. - data.first - data.second;
            auto beta = data.first;
            auto gamma = data.second;

            auto a1 = alpha / pts[0][3];
            auto b1 = beta / pts[1][3];
            auto g1 = gamma / pts[2][3];
            Vec3f bc(a1, b1, g1);
            bc = bc / (bc.x + bc.y + bc.z);

            float depth = pts[0][2] * bc.x + pts[1][2] * bc.y + pts[2][2] * bc.z;
            if (alpha < -0.01 || beta < -0.01 || gamma < -0.01) continue;
            int idx = int(x + y * width_);
            if (zBuffer_[idx] > depth) continue;
            bool discard = shader.fragment(bc, color);
            if (!discard) {
                zBuffer_[idx] = depth;
                image.set(x, y, color);
            }
        }
    }
}

Rasterizer::Rasterizer(Matrix viewport, int width, int height) :
viewport_(viewport) ,
width_(width),
height_(height)
{
    zBuffer_ = std::vector<float>(width * height, -std::numeric_limits<float>::infinity());
}

void Rasterizer::draw(Model* model, IShader& shader, TGAImage& image) {
    for (int i = 0; i < model->faceSize(); i++) {
        Vec4f screencoords[3];
        for (int j = 0; j < 3; j++) {
            screencoords[j] = viewport_ * shader.vertex(i, j);
        }
        triangle(screencoords, shader, image);
    }
}