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
            int itrash, idx, tidx;
            iss >> trash;
            while (iss >> idx >> trash >> tidx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                tidx--;
                tf.push_back(tidx);
            }
            faces_.push_back(f);
            tfaces_.push_back(tf);
        }
        else if (!line.compare(0, 2, "vt")) {
            iss >> trash >> trash;
            Vec3f vt;
            for (int i = 0; i < 3; i++) {
                iss >> vt[i];
                
            }
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

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::tface(int idx) {
    return tfaces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::tverts(int i) {
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