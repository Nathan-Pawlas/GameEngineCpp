#pragma once


class Vec2
{
public:
	Vec2() :x(0), y(0)
	{
	}
	Vec2(float x, float y) :x(x), y(y)
	{
	}
	Vec2(const Vec2& vec) :x(vec.x), y(vec.y)
	{
	}

	static Vec2 lerp(const Vec2& start, const Vec2& end, float delta)
	{
		Vec2 v;
		v.x = start.x * (1.0f - delta) + end.x * (delta);
		v.y = start.y * (1.0f - delta) + end.y * (delta);
		return v;
	}

	Vec2 operator *(float num)
	{
		return Vec2((x * num), (y * num));
	}

	Vec2 operator +(Vec2 vec)
	{
		return Vec2((x + vec.x), (y + vec.y));
	}

	~Vec2()
	{
	}

public:
	float x, y;
};
