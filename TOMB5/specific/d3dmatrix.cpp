#include "../tomb5/pch.h"
#include "d3dmatrix.h"

void SetD3DMatrixF(LPD3DMATRIX dest, float* src)
{
	D3DIdentityMatrix(dest);
	dest->_11 = src[M00];
	dest->_12 = src[M10];
	dest->_13 = src[M20];
	dest->_21 = src[M01];
	dest->_22 = src[M11];
	dest->_23 = src[M21];
	dest->_31 = src[M02];
	dest->_32 = src[M12];
	dest->_33 = src[M22];
	dest->_41 = src[M03];
	dest->_42 = src[M13];
	dest->_43 = src[M23];
}

LPD3DMATRIX D3DIdentityMatrix(LPD3DMATRIX matrix)
{
	matrix->_11 = 1;
	matrix->_12 = 0;
	matrix->_13 = 0;
	matrix->_14 = 0;
	matrix->_21 = 0;
	matrix->_22 = 1;
	matrix->_23 = 0;
	matrix->_24 = 0;
	matrix->_31 = 0;
	matrix->_32 = 0;
	matrix->_33 = 1;
	matrix->_34 = 0;
	matrix->_41 = 0;
	matrix->_42 = 0;
	matrix->_43 = 0;
	matrix->_44 = 1;
	return matrix;
}

void inject_d3dmatrix(bool replace)
{
	INJECT(0x00497550, SetD3DMatrixF, replace);
	INJECT(0x00497910, D3DIdentityMatrix, replace);
}
