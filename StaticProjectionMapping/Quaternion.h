#pragma once
//	モデル回転のための四元数（クォータニオン）の演算を定義するクラス
#include <gl\freeglut.h>
class Quaternion
{
private:
	double w, x, y, z;
	GLdouble rotateMat[16];
public:

	Quaternion()
	{
	}

	Quaternion(double ww, double xx, double yy, double zz)
	{
		w = ww; x = xx; y = yy; z = zz;
	}

	~Quaternion()
	{
	}
	//	コピーコンストラクタ
	inline Quaternion& operator=(const Quaternion &q)
	{
		this->w = q.w; this->x = q.x; this->y = q.y; this->z = q.z;
		return *this;
	}
	//	演算子のオーバーロード Quaternionの積
	inline Quaternion operator*(const Quaternion &q)
	{
		return Quaternion(
			this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z,
			this->w * q.x + this->x * q.w + this->y * q.z - this->z * q.y,
			this->w * q.y - this->x * q.z + this->y * q.w + this->z * q.x,
			this->w * q.z + this->x * q.y - this->y * q.x + this->z * q.w
		);
	}

	//	回転行列を渡す
	inline GLdouble *rotate()
	{
		double xx = this->x * this->x * 2.0;
		double yy = this->y * this->y * 2.0;
		double zz = this->z * this->z * 2.0;
		double xy = this->x * this->y * 2.0;
		double yz = this->y * this->z * 2.0;
		double zx = this->z * this->x * 2.0;
		double xw = this->x * this->w * 2.0;
		double yw = this->y * this->w * 2.0;
		double zw = this->z * this->w * 2.0;
		double r1[16] = { 
			1.0 - yy - zz, xy + zw, zx - yw, 0.0,
			xy - zw, 1.0 - zz - xx, yz + xw, 0.0,
			zx + yw, yz - xw, 1.0 - xx - yy, 0.0,
			0.0, 0.0, 0.0, 1.0 };
		for (int i = 0; i < 16; i++) {
			rotateMat[i] = r1[i];
		}
		return rotateMat;
	}
};

