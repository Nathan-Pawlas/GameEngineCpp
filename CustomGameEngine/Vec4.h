#pragma once


class Vec4
{
public:
	Vec4() :x(0), y(0), z(0), w(0)
	{
	}
	Vec4(float x, float y, float z, float w) :x(x), y(y), z(z), w(w)
	{
	}
	Vec4(const Vec4& vec) :x(vec.x), y(vec.y), z(vec.z), w(vec.w)
	{
	}

	void cross(Vec4& v1, Vec4& v2, Vec4& v3)
	{
		this->x = v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y);
		this->y = -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z));
		this->z = v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y);
		this->w = -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y));
	}

	~Vec4()
	{
	}

public:
	float x, y, z, w;
};
