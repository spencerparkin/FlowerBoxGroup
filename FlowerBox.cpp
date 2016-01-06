// FlowerBox.cpp

#include "FlowerBox.h"

FlowerBox::FlowerBox( void )
{
	int id = 0;

	for( int x = -2; x <= 2; x++ )
	{
		for( int y = -2; y <= 2; y++ )
		{
			for( int z = -2; z <= 2; z++ )
			{
				Face* face = &faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ];
				face->id = -1;
				face->color = NONE;

				if( ( -2 < y && y < 2 ) && ( -2 < z && z < 2 ) )
				{
					if( x == -2 )
						face->color = RED;
					else if( x == 2 )
						face->color = ORANGE;
				}

				if( ( -2 < y && y < 2 ) && ( -2 < x && x < 2 ) )
				{
					if( z == 2 )
						face->color = GREEN;
					else if( x == 2 )
						face->color = BLUE;
				}

				if( ( -2 < x && x < 2 ) && ( -2 < z && z < 2 ) )
				{
					if( y == -2 )
						face->color = YELLOW;
					else if( y == 2 )
						face->color = WHITE;
				}

				if( face->color )
					face->id = id++;
			}
		}
	}
}

FlowerBox::~FlowerBox( void )
{
}

void FlowerBox::Print( FILE* fp ) const
{

}

void FlowerBox::PushMatrix( const Matrix* matrix /*= nullptr*/ )
{
	if( matrix )
		matrixStack.push_back( *matrix );
	else
	{
		Matrix topMatrix;
		if( matrixStack.size() > 0 )
			topMatrix = *matrixStack.end();
		else
			for( int i = 0; i < 3; i++ )
				for( int j = 0; j < 3; j++ )
					topMatrix.ele[i][j] = ( i == j ) ? 1 : 0;

		matrixStack.push_back( topMatrix );
	}
}

void FlowerBox::PopMatrix( void )
{
	if( matrixStack.size() == 0 )
		return;

	matrixStack.pop_back();
}

void FlowerBox::LoadMatrix( const Matrix& matrix )
{
	if( matrixStack.size() == 0 )
		return;

	MatrixList::iterator iter = matrixStack.end();
	*iter = matrix;
}

void FlowerBox::MulMatrix( const Matrix& matrix )
{
	if( matrixStack.size() == 0 )
		return;

	Matrix topMatrix = *matrixStack.end();

	Matrix product;

	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			product.ele[i][j] =
				topMatrix.ele[i][0] * matrix.ele[j][0] +
				topMatrix.ele[i][1] * matrix.ele[j][1] +
				topMatrix.ele[i][2] * matrix.ele[j][2];
		}
	}

	LoadMatrix( matrix );
}

void FlowerBox::Transform( int x, int y, int z, int& tx, int& ty, int& tz )
{
	if( matrixStack.size() == 0 )
		return;

	const Matrix& topMatrix = *matrixStack.end();

	tx =
		topMatrix.ele[0][0] * x +
		topMatrix.ele[0][1] * y +
		topMatrix.ele[0][2] * z;

	ty =
		topMatrix.ele[1][0] * x +
		topMatrix.ele[1][1] * y +
		topMatrix.ele[1][2] * z;

	tz =
		topMatrix.ele[2][0] * x +
		topMatrix.ele[2][1] * y +
		topMatrix.ele[2][2] * z;
}

void FlowerBox::Matrix::SetXAxis( int x, int y, int z )
{
	ele[0][0] = x;
	ele[1][0] = y;
	ele[2][0] = z;
}

void FlowerBox::Matrix::SetYAxis( int x, int y, int z )
{
	ele[0][1] = x;
	ele[1][1] = y;
	ele[2][1] = z;
}

void FlowerBox::Matrix::SetZAxis( int x, int y, int z )
{
	ele[0][2] = x;
	ele[1][2] = y;
	ele[2][2] = z;
}

void FlowerBox::PermuteCorner( Corner corner, Rotate rotate )
{
	PushMatrix();

	switch( corner )
	{
	case CORNER_NX_NY_NZ:
	{
		// +X-axis, CW 90; +Y-axis, CW 180.
		break;
	}
	case CORNER_NX_NY_PZ:
	{
		// +Z-axis, CW 180.
		break;
	}
	case CORNER_NX_PY_NZ:
	{
		// +Y-axis, CCW 180.
		break;
	}
	case CORNER_NX_PY_PZ:
	{
		// +Y-axis, CCW 90.
		break;
	}
	case CORNER_PX_NY_NZ:
	{
		// +X-axis, CW 180.
		break;
	}
	case CORNER_PX_NY_PZ:
	{
		// +X-axis, CW 90.
		break;
	}
	case CORNER_PX_PY_NZ:
	{
		// +Y-axis, CW 90.
		break;
	}
	case CORNER_PX_PY_PZ:
	{
		break;
	}
	}

	//MulMatrix( rotationMatrix );

	int triCycle[6][3][3] =
	{
		{
			{ 2, 0, 0 }, { 0, 0, 2 }, { 0, 2, 0 }
		},
		{
			{ 2, 0, 1 }, { 0, 1, 2 }, { 1, 2, 2 }
		},
		{
			{ 2, 1, 0 }, { 1, 0, 2 }, { 0, 2, 1 }
		},
		{
			{ 2, 1, 1 }, { 1, 1, 2 }, { 1, 2, 1 }
		},
		{
			{ 2, 1, -1 }, { 1, -1, 2 }, { -1, 2, 1 }
		},
		{
			{ 2, -1, 1 }, { -1, 1, 2 }, { 1, 2, -1 }
		}
	};

	int count = ( rotate == ROTATE_CW ? 1 : 2 );
	for( int j = 0; j < count; j++ )
	{
		for( int i = 0; i < 6; i++ )
		{
			int x, y, z;

			x = triCycle[i][0][0];
			y = triCycle[i][0][1];
			z = triCycle[i][0][2];

			Transform( x, y, z, x, y, z );
			Face* faceX = &faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ];

			x = triCycle[i][1][0];
			y = triCycle[i][1][1];
			z = triCycle[i][1][2];

			Transform( x, y, z, x, y, z );
			Face* faceY = &faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ];

			x = triCycle[i][2][0];
			y = triCycle[i][2][1];
			z = triCycle[i][2][2];

			Transform( x, y, z, x, y, z );
			Face* faceZ = &faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ];

			Face tempFace = *faceX;
			*faceX = *faceY;
			*faceY = *faceZ;
			*faceZ = tempFace;
		}
	}

	PopMatrix();
}

// FlowerBox.cpp