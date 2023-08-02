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

	static Vec3 lerp(const Vec3& start, const Vec3& end, float delta)
	{
		Vec3 v;
		v.x = start.x * (1.0f - delta) + end.x * (delta);
		v.y = start.y * (1.0f - delta) + end.y * (delta);
		v.z = start.z * (1.0f - delta) + end.z * (delta);
		return v;
	}

	Vec3 operator *(float num)
	{
		return Vec3((x * num), (y * num), (z * num));
	}

	Vec3 operator +(Vec3 vec)
	{
		return Vec3((x + vec.x), (y + vec.y), (z + vec.z));
	}

	~Vec3()
	{
	}

public:
	float x, y, z;
};
