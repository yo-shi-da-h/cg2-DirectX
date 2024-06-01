#include "matrix4x4.h"

//加法
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
	

//減法
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2); 

//スカラー倍
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2); 


Matrix4x4 Inverse(const Matrix4x4& m); 


Matrix4x4 Transpose(const Matrix4x4& m); 


Matrix4x4 MakeIdentity4x4(); 

