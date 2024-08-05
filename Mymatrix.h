#pragma once
#pragma once
#include <assert.h>
#include <cmath>

struct Vector3
{
	float x;
	float y;
	float z;
};


struct Matrix4x4
{
	float m[4][4]; // 4x4行列の要素
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

Matrix4x4 add(const Matrix4x4& m1, const Matrix4x4& m2);


Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);


Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);


Matrix4x4 Inverse(const Matrix4x4& m);



Matrix4x4 Transpose(const Matrix4x4& m);


Matrix4x4 MakeIdentity4x4();


Matrix4x4 MakeTranslateMatrix(const Vector3& vector);


Matrix4x4 MakeScaleMatrix(const Vector3& vector);


Vector3 Transford(const Vector3& vector, const Matrix4x4& matrix);


Matrix4x4 MakeRoatateXMatix(float radian);



Matrix4x4 MakeRoatateYMatix(float radian);

Matrix4x4 MakeRoatateZMatix(float radian);


Matrix4x4 MakeRotateMatrix(const Vector3& radian);



Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);


//Matrix4x4 MakePerspectiveMatrix(const float& fovY, const float& aspectRatio, const float& nearClip, const float& farClip)
//{
//	Matrix4x4 result = { 0 };
//	float cot = 1.0f / tanf(fovY / 2.0f);
//
//	result.m[0][0] = cot / aspectRatio;
//
//	result.m[1][1] = cot;
//
//	result.m[2][2] = (farClip + nearClip) / (nearClip - farClip);
//
//	result.m[2][3] = 1.0f;
//
//	result.m[3][2] = (farClip * nearClip) / (nearClip - farClip);
//
//	return result;
//}

Matrix4x4 MakePerspectiveFovMatrix(const float fovY, const float aspectRatio, const float nearClip, const float farClip);




Matrix4x4 MakeOrthogphicMatrix(const float& left, const float& top, const float& right, const float& bottom, const float& nearClip, const float& farClip);


Matrix4x4 MakeViewportMatrix(const float& left, const float& top, const float& width, const float& height, const float& minDepth, const float& maxDepth);

