#pragma once


class Vec3
{
public:
	Vec3():x(0),y(0),z(0)
	{
	}
	Vec3(float x, float y, float z) :x(x), y(y), z(z)
	{
	}
	Vec3(const Vec3& vec) :x(vec.x), y(vec.y), z(vec.z)
	{
	}

	~Vec3()
	{
	}

public:
	float x, y, z;
};
