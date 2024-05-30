#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<std::vector<int> > tfaces_;
	std::vector<std::vector<int> > nfaces_;
	std::vector<Vec2f> tverts_;
	std::vector<Vec3f> nverts_;
	float maxNum_;
	TGAImage diffuseTex_;
	TGAImage normalTex_;
	TGAImage specularTex_;
	
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int faceSize();
	Vec3f vert(int i);
	Vec2f tverts(int i);
	Vec3f nverts(int i);
	std::vector<int> face(int idx);
	std::vector<int> tface(int idx);
	std::vector<int> nface(int idx);
	void loadDiffuseTexture(const char* filename);
	void loadNormalTexture(const char* filename);
	void loadSpecularTexture(const char* filename);
	TGAColor getDiffuseColor(float x, float y);
	Vec3f getNormal(float x, float y);
	float getSpecularColor(float x, float y);
	float getMaxNum();
};

#endif //__MODEL_H__
