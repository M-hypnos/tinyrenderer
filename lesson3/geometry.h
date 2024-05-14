#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <cassert>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	Vec2<t>(const Vec2<t>& rhs) :u(rhs.u), v(rhs.v) {}

	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	t& operator[](const int i) {
		if (i <= 0) return x;
		else return y;
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	template<class u> Vec3<t>(const Vec3<u>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {};
	Vec3<t>(const Vec3<t>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {};

	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	t& operator[](const int i) {
		if (i <= 0) return x;
		else if (i == 1) return y;
		else return z;
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

class Matrix {
	std::vector<std::vector<float>> m;
	int row, col;
public:
	Matrix(int r = 4, int c = 4) :m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))), row(r), col(c) {};
	int nrow() { return row; }
	int ncol() { return col; }
	std::vector<float>& operator[](const int i) {
		assert(i >= 0 && i < row);
		return m[i];
	}
	static Matrix identity(int dimensions) {
		Matrix e(dimensions, dimensions);
		for (int r = 0; r < dimensions; r++) {
			for (int c = 0; c < dimensions; c++) {
				e[r][c] = (r == c ? 1.f : 0.f);
			}
		}
		return e;
	}
	Matrix operator*(const Matrix& a) {
		assert(col == a.row);
		Matrix ans(row, a.col);
		for (int r = 0; r < row; r++) {
			for (int c = 0; c < a.col; c++) {
				ans[r][c] = 0.f;
				for (int i = 0; i < col; i++) {
					ans[r][c] += m[r][i] * a.m[i][c];
				}
			}
		}
		return ans;
	}
	Matrix transpose() {
		Matrix ans(row, col);
		for (int r = 0; r < row; r++) {
			for (int c = 0; c < col; c++) {
				ans[r][c] = m[c][r];
			}
		}
		return ans;
	}
	Matrix inverse() {
		assert(row == col);
		Matrix result(row, col * 2);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				result[i][j] = m[i][j];
		for (int i = 0; i < row; i++)
			result[i][i + col] = 1;
		// first pass
		for (int i = 0; i < row - 1; i++) {
			// normalize the first row
			for (int j = result.col - 1; j >= 0; j--)
				result[i][j] /= result[i][i];
			for (int k = i + 1; k < row; k++) {
				float coeff = result[k][i];
				for (int j = 0; j < result.col; j++) {
					result[k][j] -= result[i][j] * coeff;
				}
			}
		}
		// normalize the last row
		for (int j = result.col - 1; j >= row - 1; j--)
			result[row - 1][j] /= result[row - 1][row - 1];
		// second pass
		for (int i = row - 1; i > 0; i--) {
			for (int k = i - 1; k >= 0; k--) {
				float coeff = result[k][i];
				for (int j = 0; j < result.col; j++) {
					result[k][j] -= result[i][j] * coeff;
				}
			}
		}
		// cut the identity matrix back
		Matrix truncate(row, col);
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++)
				truncate[i][j] = result[i][j + col];
		return truncate;
	}
};

#endif //__GEOMETRY_H__
