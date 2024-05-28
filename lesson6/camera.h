#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "geometry.h"

class Camera
{
public:
	Camera(Vec3f cameraPos, Vec3f target, Vec3f up):cameraPos_(cameraPos), target_(target), up_(up) {}
	Matrix getView();
	Matrix getProjection();
private:
	Vec3f cameraPos_;
	Vec3f target_;
	Vec3f up_;
};

#endif