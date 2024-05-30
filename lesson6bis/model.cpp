#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), tverts_(), nverts_(), faces_(), tfaces_(), maxNum_(1.) {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) {
                iss >> v[i];
                if (std::abs(v[i]) > maxNum_) {
                    maxNum_ = std::abs(v[i]);
                }
            }
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> tf;
            std::vector<int> nf;
            int idx, tidx, nidx;
            iss >> trash;
            while (iss >> idx >> trash >> tidx >> trash >> nidx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                tidx--;
                tf.push_back(tidx);
                nidx--;
                nf.push_back(nidx);
            }
            faces_.push_back(f);
            tfaces_.push_back(tf);
            nfaces_.push_back(nf);
        }
        else if (!line.compare(0, 2, "vt")) {
            iss >> trash >> trash;
            Vec2f vt;
            for (int i = 0; i < 2; i++) {
                iss >> vt[i];
                
            }
            iss >> trash;
            tverts_.push_back(vt);
        }
        else if (!line.compare(0, 2, "vn")) {
            iss >> trash >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++) {
                iss >> vn[i];
                
            }
            nverts_.push_back(vn);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::faceSize() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::tface(int idx) {
    return tfaces_[idx];
}

std::vector<int> Model::nface(int idx) {
    return nfaces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::tverts(int i) {
    return tverts_[i];
}

Vec3f Model::nverts(int i) {
    return nverts_[i];
}

float Model::getMaxNum() {
    return maxNum_;
}

void Model::loadDiffuseTexture(const char* filename) {
    diffuseTex_.read_tga_file(filename);
    diffuseTex_.flip_vertically();
}

TGAColor Model::getDiffuseColor(float x, float y) {
    return diffuseTex_.get(x * diffuseTex_.get_width(), y * diffuseTex_.get_height());
}

void Model::loadNormalTexture(const char* filename) {
    normalTex_.read_tga_file(filename);
    normalTex_.flip_vertically();
}

Vec3f Model::getNormal(float x, float y) {
    TGAColor n = normalTex_.get(x * normalTex_.get_width(), y * normalTex_.get_height());
    Vec3f res;
    for (int i = 0; i < 3; i++) {
        res[2-i] = n[i] / 255.f * 2 - 1.f;
    }
    return res;
}

void Model::loadSpecularTexture(const char* filename) {
    specularTex_.read_tga_file(filename);
    specularTex_.flip_vertically();
}

float Model::getSpecularColor(float x, float y) {
    return specularTex_.get(x * specularTex_.get_width(), y * specularTex_.get_height())[0] / 1.f;
}