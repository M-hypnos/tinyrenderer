#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "camera.h"
#include "rasterizer.h"

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

Model* model = nullptr;
Camera* camera = nullptr;
Rasterizer* rasterizer = nullptr;
Vec3f light_dir = Vec3f(1, 1, 1).normalize();

Matrix viewport(int x, int y, int width, int height) {
    Matrix m = Matrix::identity();
    m[0][0] = width / 2.f;
    m[1][1] = height / 2.f;
    m[2][2] = depth / 2.f;
    m[0][3] = width / 2.f + x;
    m[1][3] = height / 2.f + y;
    m[2][3] = depth / 2.f;
    return m;
}

struct GouraudShader:public IShader
{
    Vec3f intensity;

    virtual Vec4f vertex(int iface, int vertIdx) {
        Vec3f n = model->nverts(model->nface(iface)[vertIdx]);
        intensity[vertIdx] = std::max(0.f, n * light_dir);
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        float I = intensity * barycentricCoordinates;
        //color = TGAColor(255, 255, 255) * I;
        if (I > .85) I = 1;
        else if (I > .60) I = .80;
        else if (I > .45) I = .60;
        else if (I > .30) I = .45;
        else if (I > .15) I = .30;
        else I = 0;
        color = TGAColor(255, 155, 0) * I;
        return false;
    }
};

struct FlatShader :public IShader
{
    Vec3f vert[3];
    virtual Vec4f vertex(int iface, int vertIdx) {

        vert[vertIdx] = model->vert(model->face(iface)[vertIdx]);
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(vert[vertIdx]);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec3f n = cross((vert[1] - vert[0]), (vert[2] - vert[0])).normalize();
        float I = std::max(0.f, n * light_dir);
        color = TGAColor(255, 255, 255) * I;
        return false;
    }
};

struct PhongShader :public IShader
{
    Vec3f n[3];
    virtual Vec4f vertex(int iface, int vertIdx) {
        n[vertIdx] = model->nverts(model->nface(iface)[vertIdx]);
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec3f normal = n[0] * barycentricCoordinates.x + n[1] * barycentricCoordinates.y + n[2] * barycentricCoordinates.z;
        float I = std::max(0.f, normal * light_dir);
        color = TGAColor(255, 255, 255) * I;
        return false;
    }
};

struct GouraudTexShader :public IShader
{
    Vec3f intensity;
    mat<2, 3, float> uv;
    virtual Vec4f vertex(int iface, int vertIdx) {
        Vec3f n = model->nverts(model->nface(iface)[vertIdx]);
        intensity[vertIdx] = std::max(0.f, n * light_dir);
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        uv.set_col(vertIdx, model->tverts(model->tface(iface)[vertIdx]));
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        float I = intensity * barycentricCoordinates;
        Vec2f texcoords = uv * barycentricCoordinates;
        color = model->getDiffuseColor(texcoords.x, texcoords.y) * I;
        return false;
    }
};

struct FlatTexShader :public IShader
{
    Vec3f vert[3];
    mat<2, 3, float> uv;
    virtual Vec4f vertex(int iface, int vertIdx) {
        vert[vertIdx] = model->vert(model->face(iface)[vertIdx]);
        uv.set_col(vertIdx, model->tverts(model->tface(iface)[vertIdx]));
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(vert[vertIdx]);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec3f n = cross((vert[1] - vert[0]), (vert[2] - vert[0])).normalize();
        float I = std::max(0.f, n * light_dir);
        Vec2f texcoords = uv * barycentricCoordinates;
        color = model->getDiffuseColor(texcoords.x, texcoords.y) * I;
        return false;
    }
};

struct PhongTexShader :public IShader
{
    Vec3f n[3];
    mat<2, 3, float> uv;
    virtual Vec4f vertex(int iface, int vertIdx) {
        n[vertIdx] = model->nverts(model->nface(iface)[vertIdx]);
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        uv.set_col(vertIdx, model->tverts(model->tface(iface)[vertIdx]));
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec3f normal = n[0] * barycentricCoordinates.x + n[1] * barycentricCoordinates.y + n[2] * barycentricCoordinates.z;
        float I = std::max(0.f, normal * light_dir);
        Vec2f texcoords = uv * barycentricCoordinates;
        color = model->getDiffuseColor(texcoords.x, texcoords.y) * I;
        return false;
    }
};

struct normalTexShader :public IShader {
    mat<2, 3, float> uv;
    Matrix MVP = rasterizer->getProjection() * rasterizer->getModelView();
    Matrix MVPT = MVP.invert_transpose();
    virtual Vec4f vertex(int iface, int vertIdx) {
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        uv.set_col(vertIdx, model->tverts(model->tface(iface)[vertIdx]));
        return rasterizer->getProjection() * rasterizer->getModelView() * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec2f texcoords = uv * barycentricCoordinates;
        Vec3f normal = model->getNormal(texcoords.x, texcoords.y);

        Vec3f n = proj<3>(MVPT * embed<4>(normal)).normalize();
        Vec3f l = proj<3>(MVP * embed<4>(light_dir)).normalize();

        float I = std::max(0.f, n * l);
        color = model->getDiffuseColor(texcoords.x, texcoords.y) * I;
        return false;
    }
};

struct Shader : public IShader {
    mat<2, 3, float> uv;
    Matrix MVP = rasterizer->getProjection() * rasterizer->getModelView();
    Matrix MVPT = MVP.invert_transpose();
    virtual Vec4f vertex(int iface, int vertIdx) {
        Vec3f v = model->vert(model->face(iface)[vertIdx]);
        uv.set_col(vertIdx, model->tverts(model->tface(iface)[vertIdx]));
        return MVP * embed<4>(v);
    }
    virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) {
        Vec2f texcoords = uv * barycentricCoordinates;
        Vec3f normal = model->getNormal(texcoords.x, texcoords.y);

        Vec3f n = proj<3>(MVPT * embed<4>(normal)).normalize();
        Vec3f l = proj<3>(MVP * embed<4>(light_dir)).normalize();

        Vec3f r = (n * (n * l * 2.f) - l).normalize();
        float spec = pow(std::max(r.z, 0.0f), model->getSpecularColor(texcoords.x, texcoords.y));
        float diff = std::max(0.f, n * l);
        TGAColor c = model->getDiffuseColor(texcoords.x, texcoords.y);
        color = c;
        for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + .6 * spec), 255);
        return false;
    }
};

int main(int argc, char** argv) {
    TGAImage image(Width, Height, TGAImage::RGB);

    model = new Model("obj/african_head.obj");
    model->loadDiffuseTexture("obj/african_head_diffuse.tga");
    model->loadNormalTexture("obj/african_head_nm.tga");
    model->loadSpecularTexture("obj/african_head_spec.tga");
    camera = new Camera(Vec3f(1, 1, 3), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
    rasterizer = new Rasterizer(viewport(Width / 8, Height / 8, Width * 3 / 4, Height * 3 / 4), Width, Height);

    rasterizer->setModelView(camera->getView());
    rasterizer->setProjection(camera->getProjection());
    
    GouraudShader gshader;
    FlatShader fshader;
    PhongShader pshader;
    GouraudTexShader gtshader;
    FlatTexShader ftshader;
    PhongTexShader ptshader;
    normalTexShader nshader;
    Shader shader;
    rasterizer->draw(model, shader, image);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}

