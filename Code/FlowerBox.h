// FlowerBox.h

#pragma once

#include <stdio.h>
#include <list>

class FlowerBox
{
public:

	FlowerBox( void );
	~FlowerBox( void );

	void Draw( void );

	enum Color
	{
		NONE,
		WHITE,
		YELLOW,
		RED,
		ORANGE,
		BLUE,
		GREEN,
	};

	struct Face
	{
		int id;
		Color color;
	};

	Face faceMatrix[5][5][5];

	struct Matrix
	{
		int ele[3][3];

		void SetXAxis( int x, int y, int z );
		void SetYAxis( int x, int y, int z );
		void SetZAxis( int x, int y, int z );

		void Transpose( void );
	};

	typedef std::list< Matrix > MatrixList;
	MatrixList matrixStack;

	void PushMatrix( const Matrix* matrix = nullptr );
	void PopMatrix( void );
	void LoadMatrix( const Matrix& matrix );
	void MulMatrix( const Matrix& matrix );
	void Transform( int x, int y, int z, int& tx, int& ty, int& tz );

	enum Rotate
	{
		ROTATE_CW,
		ROTATE_CCW,
	};

	enum Corner
	{
		CORNER_NX_NY_NZ,
		CORNER_NX_NY_PZ,
		CORNER_NX_PY_NZ,
		CORNER_NX_PY_PZ,
		CORNER_PX_NY_NZ,
		CORNER_PX_NY_PZ,
		CORNER_PX_PY_NZ,
		CORNER_PX_PY_PZ,
	};

	void PermuteCorner( Corner corner, Rotate rotate );
};

// FlowerBox.h
