#pragma once
#include "Vec3.h"
#include <memory>

class Mat4
{
public:
	Mat4()
	{
		setIdentity();
	}
	
	void setIdentity()
	{
		::memset(mat, 0, sizeof(float) * 16);
		mat[0][0] = 1;
		mat[1][1] = 1;
		mat[2][2] = 1;
		mat[3][3] = 1;
	}

	void setTranslation(const Vec3& translation)
	{
		setIdentity();
		mat[3][0] = translation.x;
		mat[3][1] = translation.y;
		mat[3][2] = translation.z;
	}

	void setScale(const Vec3& scale)
	{
		setIdentity();
		mat[0][0] = scale.x;
		mat[1][1] = scale.y;
		mat[2][2] = scale.z;
	}

	void operator *=(const Mat4& matrix)
	{
		Mat4 out;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				out.mat[i][j] =
					mat[i][0] * matrix.mat[0][j] + mat[i][1] * matrix.mat[1][j] +
					mat[i][2] * matrix.mat[2][j] + mat[i][3] * matrix.mat[3][j];
			}
		}
		setMatrix(out);
	}

	void setMatrix(const Mat4& matrix)
	{
		::memcpy(mat, matrix.mat, sizeof(float) * 16);
	}

	void setOrthoLH(float width, float height, float near_plane, float far_plane)
	{
		setIdentity();
		mat[0][0] = 2.0f / width;
		mat[1][1] = 2.0f / height;
		mat[2][2] = 1.0f / (far_plane - near_plane);
		mat[3][2] = -(near_plane / (far_plane - near_plane));
	}

	~Mat4()
	{}
	
public:
	float mat[4][4];
};
