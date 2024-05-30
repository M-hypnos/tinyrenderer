#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__
#include "geometry.h"
#include "tgaimage.h"
#include "model.h"

//extern Camera camera;

struct IShader
{
	virtual Vec4f vertex(int iface, int vertIdx) = 0;
	virtual bool fragment(Vec3f barycentricCoordinates, TGAColor& color) = 0;
};

#pragma once
class Rasterizer
{
public:
	Rasterizer(Matrix viewport, int width, int height);
	void setModelView(Matrix modelView) { modelView_ = modelView; }
	void setProjection(Matrix projection) { projection_ = projection; }
	void draw(Model* model, IShader& shader, TGAImage& image);
	Matrix getProjection() { return projection_; }
	Matrix getModelView() { return modelView_; }
private:
	void triangle(Vec4f* pts, IShader& shader, TGAImage& image);

	Matrix viewport_;
	Matrix modelView_;
	Matrix projection_;
	std::vector<float> zBuffer_;
	int width_;
	int height_;
};

#endif
