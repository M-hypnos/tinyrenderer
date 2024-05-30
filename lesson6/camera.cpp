#include "camera.h"

Matrix Camera::getView() {
    Vec3f z = (cameraPos_ - target_).normalize();
    Vec3f x = (cross(up_, z)).normalize();
    Vec3f y = (cross(z, x)).normalize();

    Matrix t = Matrix::identity();
    t[0][3] = -target_.x;
    t[1][3] = -target_.y;
    t[2][3] = -target_.z;

    Matrix t1 = Matrix::identity();
    for (int i = 0; i < 3; i++) {
        t1[0][i] = x[i];
        t1[1][i] = y[i];
        t1[2][i] = z[i];
    }
    return t1 * t;
}

Matrix Camera::getProjection() {
    Matrix projection = Matrix::identity();
    projection[3][2] = -1.f / (cameraPos_ - target_).norm();
    return projection;
}