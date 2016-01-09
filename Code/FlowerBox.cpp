// FlowerBox.cpp

#include "FlowerBox.h"
#include <wx/glcanvas.h>
#include <math.h>

FlowerBox::FlowerBox( void )
{
	selectedFaceId = -1;

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
					if( z == -2 )
						face->color = GREEN;
					else if( z == 2 )
						face->color = BLUE;
				}

				if( ( -2 < x && x < 2 ) && ( -2 < z && z < 2 ) )
				{
					if( y == -2 )
						face->color = WHITE;
					else if( y == 2 )
						face->color = YELLOW;
				}

				if( face->color )
					face->id = id++;
			}
		}
	}

	for( int i = 0; i < CORNER_COUNT; i++ )
		cornerRotationAngles[i] = 0.0;

	ModelFace( RED );
	ModelFace( ORANGE );
	ModelFace( GREEN );
	ModelFace( BLUE );
	ModelFace( WHITE );
	ModelFace( YELLOW );

	for( int i = 0; i < ( signed )polygonArray.size(); i++ )
	{
		Polygon& polygon = polygonArray[i];

		if( polygon.x <= 0 && polygon.y <= 0 && polygon.z <= 0 )
			polygon.boundCorners.push_back( CORNER_NX_NY_NZ );

		if( polygon.x <= 0 && polygon.y <= 0 && polygon.z >= 0 )
			polygon.boundCorners.push_back( CORNER_NX_NY_PZ );

		if( polygon.x <= 0 && polygon.y >= 0 && polygon.z <= 0 )
			polygon.boundCorners.push_back( CORNER_NX_PY_NZ );

		if( polygon.x <= 0 && polygon.y >= 0 && polygon.z >= 0 )
			polygon.boundCorners.push_back( CORNER_NX_PY_PZ );

		if( polygon.x >= 0 && polygon.y <= 0 && polygon.z <= 0 )
			polygon.boundCorners.push_back( CORNER_PX_NY_NZ );

		if( polygon.x >= 0 && polygon.y <= 0 && polygon.z >= 0 )
			polygon.boundCorners.push_back( CORNER_PX_NY_PZ );

		if( polygon.x >= 0 && polygon.y >= 0 && polygon.z <= 0 )
			polygon.boundCorners.push_back( CORNER_PX_PY_NZ );

		if( polygon.x >= 0 && polygon.y >= 0 && polygon.z >= 0 )
			polygon.boundCorners.push_back( CORNER_PX_PY_PZ );

		for( int j = 0; j < CORNER_COUNT; j++ )
		{
			Matrix rotationMatrix;
			CalcMatrixForCorner( Corner(j), rotationMatrix );
			//rotationMatrix.Transpose();
			PushMatrix( &rotationMatrix );

			int x, y, z;
			Transform( polygon.x, polygon.y, polygon.z, x, y, z );

			if( ( x == 2 && y == -1 && z == 1 ) ||
				( x == 2 && y == 1 && z == -1 ) ||
				( x == -1 && y == 2 && z == 1 ) ||
				( x == 1 && y == 2 && z == -1 ) ||
				( x == 1 && y == -1 && z == 2 ) ||
				( x == -1 && y == 1 && z == 2 ) )
			{
				polygon.boundCorners.push_back( Corner(j) );
			}

			PopMatrix();
		}
	}
}

FlowerBox::~FlowerBox( void )
{
}

void FlowerBox::ModelFace( Color color )
{
	VertexBuffer vertexBuffer;

	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, -1.0, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, -1.0, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 1.0, 1.0 ) );

	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -0.4, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.4, 0.0, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.4, 1.0 ) );
	vertexBuffer.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, -0.4, 0.0, 1.0 ) );

	c3ga::rotorE3GA rotor;

	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	switch( color )
	{
		case RED:			rotor = AxisAngleRotor( yAxis, -90.0 );		break;
		case ORANGE:		rotor = AxisAngleRotor( yAxis, 90.0 );		break;
		case GREEN:			rotor = AxisAngleRotor( yAxis, 180.0 );		break;
		case BLUE:			rotor = AxisAngleRotor( xAxis, 0.0 );		break;
		case WHITE:			rotor = AxisAngleRotor( xAxis, 90.0 );		break;
		case YELLOW:		rotor = AxisAngleRotor( xAxis, -90.0 );		break;
	}

	VertexBuffer polygonA;
	polygonA.push_back( vertexBuffer[0] );
	polygonA.push_back( vertexBuffer[7] );
	polygonA.push_back( vertexBuffer[3] );
	ModelFace( polygonA, rotor, -1, 0, 2 );

	VertexBuffer polygonB;
	polygonB.push_back( vertexBuffer[0] );
	polygonB.push_back( vertexBuffer[1] );
	polygonB.push_back( vertexBuffer[4] );
	ModelFace( polygonB, rotor, 0, -1, 2 );

	VertexBuffer polygonC;
	polygonC.push_back( vertexBuffer[1] );
	polygonC.push_back( vertexBuffer[2] );
	polygonC.push_back( vertexBuffer[5] );
	ModelFace( polygonC, rotor, 1, 0, 2 );

	VertexBuffer polygonD;
	polygonD.push_back( vertexBuffer[2] );
	polygonD.push_back( vertexBuffer[3] );
	polygonD.push_back( vertexBuffer[6] );
	ModelFace( polygonD, rotor, 0, 1, 2 );

	VertexBuffer polygonE;
	polygonE.push_back( vertexBuffer[0] );
	polygonE.push_back( vertexBuffer[4] );
	polygonE.push_back( vertexBuffer[7] );
	ModelFace( polygonE, rotor, -1, -1, 2 );

	VertexBuffer polygonF;
	polygonF.push_back( vertexBuffer[4] );
	polygonF.push_back( vertexBuffer[1] );
	polygonF.push_back( vertexBuffer[5] );
	ModelFace( polygonF, rotor, 1, -1, 2 );

	VertexBuffer polygonG;
	polygonG.push_back( vertexBuffer[5] );
	polygonG.push_back( vertexBuffer[2] );
	polygonG.push_back( vertexBuffer[6] );
	ModelFace( polygonG, rotor, 1, 1, 2 );

	VertexBuffer polygonH;
	polygonH.push_back( vertexBuffer[6] );
	polygonH.push_back( vertexBuffer[3] );
	polygonH.push_back( vertexBuffer[7] );
	ModelFace( polygonH, rotor, -1, 1, 2 );

	VertexBuffer polygonI;
	polygonI.push_back( vertexBuffer[4] );
	polygonI.push_back( vertexBuffer[5] );
	polygonI.push_back( vertexBuffer[6] );
	polygonI.push_back( vertexBuffer[7] );
	ModelFace( polygonI, rotor, 0, 0, 2 );
}

void FlowerBox::ModelFace( const VertexBuffer& polygonVerts, const c3ga::rotorE3GA& rotor, int x, int y, int z )
{
	Polygon polygon;

	c3ga::vectorE3GA indexVector( c3ga::vectorE3GA::coord_e1_e2_e3, x, y, z );
	indexVector = c3ga::applyUnitVersor( rotor, indexVector );

	polygon.x = int( ::round( indexVector.m_e1 ) );
	polygon.y = int( ::round( indexVector.m_e2 ) );
	polygon.z = int( ::round( indexVector.m_e3 ) );

	for( int i = 0; i < ( signed )polygonVerts.size(); i++ )
	{
		c3ga::vectorE3GA vertex = polygonVerts[i];
		vertex = c3ga::applyUnitVersor( rotor, vertex );
		polygon.indexBuffer.push_back( GetIndexForVertex( vertex ) );
	}

	polygonArray.push_back( polygon );
}

int FlowerBox::GetIndexForVertex( const c3ga::vectorE3GA& vertex )
{
	double eps = 1e-5;
	for( int i = 0; i < ( signed )vertexBuffer.size(); i++ )
		if( c3ga::norm( vertexBuffer[i] - vertex ) < eps )
			return i;
	
	vertexBuffer.push_back( vertex );
	return int( vertexBuffer.size() - 1 );
}

/*static*/ c3ga::rotorE3GA FlowerBox::AxisAngleRotor( const c3ga::vectorE3GA& unitAxis, double angleDegrees )
{
	double halfAngleRadians = ( M_PI / 180.0 ) * angleDegrees * 0.5;
	double cosHalfAngle = cos( halfAngleRadians );
	double sinHalfAngle = sin( halfAngleRadians );

	c3ga::rotorE3GA rotor;

	rotor.m_scalar = cosHalfAngle;
	rotor.m_e2_e3 = -sinHalfAngle * unitAxis.m_e1;
	rotor.m_e3_e1 = -sinHalfAngle * unitAxis.m_e2;
	rotor.m_e1_e2 = -sinHalfAngle * unitAxis.m_e3;

	return rotor;
}

/*static*/ c3ga::vectorE3GA FlowerBox::CornerAxis( Corner corner )
{
	c3ga::vectorE3GA axis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );

	switch( corner )
	{
		case CORNER_NX_NY_NZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, -1.0, -1.0 ); break;
		case CORNER_NX_NY_PZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, -1.0, 1.0 ); break;
		case CORNER_NX_PY_NZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 1.0, -1.0 ); break;
		case CORNER_NX_PY_PZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 1.0, 1.0 ); break;
		case CORNER_PX_NY_NZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, -1.0, -1.0 ); break;
		case CORNER_PX_NY_PZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, -1.0, 1.0 ); break;
		case CORNER_PX_PY_NZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, -1.0 ); break;
		case CORNER_PX_PY_PZ: axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 ); break;
	}

	axis = c3ga::unit( axis );
	return axis;
}

void FlowerBox::Draw( GLenum renderMode )
{
	int count = ( signed )polygonArray.size();
	for( int i = 0; i < count; i++ )
	{
		Polygon& polygon = polygonArray[i];
		polygon.Draw( this, renderMode );
	}
}

void FlowerBox::ProcessHitBuffer( GLuint* hitBuffer, GLuint hitBufferSize, GLint hitCount )
{
	selectedFaceId = -1;

	// Notice that if "hitCount" is -1, which indicates an
	// over-flow of the buffer, we don't process anything.
	unsigned int* hitRecord = hitBuffer;
	float smallestZ = 999.f;
	for( int hit = 0; hit < hitCount; hit++ )
	{
		unsigned int nameCount = hitRecord[0];
		wxASSERT( nameCount == 1 );
		if( nameCount == 1 )
		{
			float minZ = float( hitRecord[1] ) / float( 0x7FFFFFFF );
			if( minZ < smallestZ )
			{
				smallestZ = minZ;
				selectedFaceId = ( int )hitRecord[3];
			}
		}

		hitRecord += 3 + nameCount;
	}
}

FlowerBox::Corner FlowerBox::ClosestCornerOfFace( int faceId )
{
	int x, y, z;
	Face* face = FindFace( faceId, x, y, z );
	if( !face )
		return CORNER_COUNT;

	if( x < 0 && y < 0 && z < 0 )	return CORNER_NX_NY_NZ;
	if( x < 0 && y < 0 && z > 0 )	return CORNER_NX_NY_PZ;
	if( x < 0 && y > 0 && z < 0 )	return CORNER_NX_PY_NZ;
	if( x < 0 && y > 0 && z > 0 )	return CORNER_NX_PY_PZ;
	if( x > 0 && y < 0 && z < 0 )	return CORNER_PX_NY_NZ;
	if( x > 0 && y < 0 && z > 0 )	return CORNER_PX_NY_PZ;
	if( x > 0 && y > 0 && z < 0 )	return CORNER_PX_PY_NZ;
	if( x > 0 && y > 0 && z > 0 )	return CORNER_PX_PY_PZ;
	
	return CORNER_COUNT;
}

FlowerBox::Face* FlowerBox::FindFace( int faceId, int& x, int& y, int& z )
{
	for( x = 0; x < 5; x++ )
	{
		for( y = 0; y < 5; y++ )
		{
			for( z = 0; z < 5; z++ )
			{
				Face* face = &faceMatrix[x][y][z];
				if( face->id == faceId )
				{
					x -= 2;
					y -= 2;
					z -= 2;
					return face;
				}
			}
		}
	}

	return nullptr;
}

void FlowerBox::Polygon::Draw( FlowerBox* flowerBox, GLenum renderMode )
{
	Face& face = flowerBox->faceMatrix[ x + 2 ][ y + 2 ][ z + 2 ];

	if( renderMode == GL_SELECT )
		glLoadName( face.id );

	c3ga::rotorE3GA rotor( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );

	// No more than one corner will ever be rotating at once, but this just seems more correct to me.
	// If we did simulate trying to pull a face in multiple direction with two adjacent corner rotations,
	// then we would really need to weight the face to each corner/rotor by some amount.
	for( int i = 0; i < ( signed )boundCorners.size(); i++ )
	{
		int j = boundCorners[i];
		double angleDegrees = flowerBox->cornerRotationAngles[j];
		if( angleDegrees != 0.0 )
		{
			c3ga::rotorE3GA cornerRotor = AxisAngleRotor( CornerAxis( Corner(j) ), angleDegrees );
			rotor = rotor * cornerRotor;
		}
	}

	VertexBuffer polygonVerts;
	c3ga::vectorE3GA center( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	glColor3f( 0.f, 0.f, 0.f );
	//glBegin( GL_POLYGON );

	for( int i = 0; i < ( signed )indexBuffer.size(); i++ )
	{
		int j = indexBuffer[i];
		c3ga::vectorE3GA vertex = flowerBox->vertexBuffer[j];
		vertex = c3ga::applyUnitVersor( rotor, vertex );
		polygonVerts.push_back( vertex );
		center = center + vertex;

		//glVertex3f( vertex.m_e1, vertex.m_e2, vertex.m_e3 );
	}

	//glEnd();

	center = center * ( 1.0 / double( indexBuffer.size() ) );

	switch( face.color )
	{
		case WHITE:		glColor3f( 1.f, 1.f, 1.f );		break;
		case YELLOW:	glColor3f( 1.f, 1.f, 0.f );		break;
		case RED:		glColor3f( 1.f, 0.f, 0.f );		break;
		case ORANGE:	glColor3f( 1.f, 0.6f, 0.f );	break;
		case BLUE:		glColor3f( 0.f, 0.f, 1.f );		break;
		case GREEN:		glColor3f( 0.f, 1.f, 0.f );		break;
		default:		glColor3f( 0.5f, 0.5f, 0.5f );	break;
	}

	//wxASSERT( polgyonVerts.size() > 1 );
	c3ga::vectorE3GA normal = c3ga::op( polygonVerts[0] - center, polygonVerts[1] - center ) * c3ga::I3;
	normal = c3ga::unit( normal );

	glBegin( GL_POLYGON );

	for( int i = 0; i < ( signed )polygonVerts.size(); i++ )
	{
		c3ga::vectorE3GA vertex = polygonVerts[i];
		vertex = center + ( vertex - center ) * 0.9;
		//vertex = vertex + normal * -0.02;
		glVertex3f( vertex.m_e1, vertex.m_e2, vertex.m_e3 );
	}

	glEnd();
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
	iter--;
	*iter = matrix;
}

void FlowerBox::MulMatrix( const Matrix& matrix )
{
	if( matrixStack.size() == 0 )
		return;

	Matrix topMatrix = matrixStack.back();

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

	const Matrix& topMatrix = matrixStack.back();

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

void FlowerBox::Matrix::Transpose( void )
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < i; j++ )
		{
			int e = ele[i][j];
			ele[i][j] = ele[j][i];
			ele[j][i] = e;
		}
	}
}

bool FlowerBox::CalcMatrixForCorner( Corner corner, Matrix& rotationMatrix )
{
	// We must position the corner in the postive octant.
	// The orientation of the corner doesn't matter.
	switch( corner )
	{
		case CORNER_NX_NY_NZ:
		{
			rotationMatrix.SetXAxis( -1, 0, 0 );
			rotationMatrix.SetYAxis( 0, 0, -1 );
			rotationMatrix.SetZAxis( 0, -1, 0 );
			return true;
		}
		case CORNER_NX_NY_PZ:
		{
			rotationMatrix.SetXAxis( -1, 0, 0 );
			rotationMatrix.SetYAxis( 0, -1, 0 );
			rotationMatrix.SetZAxis( 0, 0, 1 );
			return true;
		}
		case CORNER_NX_PY_NZ:
		{
			rotationMatrix.SetXAxis( -1, 0, 0 );
			rotationMatrix.SetYAxis( 0, 1, 0 );
			rotationMatrix.SetZAxis( 0, 0, -1 );
			return true;
		}
		case CORNER_NX_PY_PZ:
		{
			rotationMatrix.SetXAxis( 0, 0, -1 );
			rotationMatrix.SetYAxis( 0, 1, 0 );
			rotationMatrix.SetZAxis( 1, 0, 0 );
			return true;
		}
		case CORNER_PX_NY_NZ:
		{
			rotationMatrix.SetXAxis( 1, 0, 0 );
			rotationMatrix.SetYAxis( 0, -1, 0 );
			rotationMatrix.SetZAxis( 0, 0, -1 );
			return true;
		}
		case CORNER_PX_NY_PZ:
		{
			rotationMatrix.SetXAxis( 1, 0, 0 );
			rotationMatrix.SetYAxis( 0, 0, -1 );
			rotationMatrix.SetZAxis( 0, 1, 0 );
			return true;
		}
		case CORNER_PX_PY_NZ:
		{
			rotationMatrix.SetXAxis( 1, 0, 0 );
			rotationMatrix.SetYAxis( 0, 0, 1 );
			rotationMatrix.SetZAxis( 0, -1, 0 );
			return true;
		}
		case CORNER_PX_PY_PZ:
		{
			rotationMatrix.SetXAxis( 1, 0, 0 );
			rotationMatrix.SetYAxis( 0, 1, 0 );
			rotationMatrix.SetZAxis( 0, 0, 1 );
			return true;
		}
	}

	return false;
}

void FlowerBox::PermuteCorner( Corner corner, Rotate rotate, bool adjustCornerRotation )
{
	if( adjustCornerRotation )
	{
		if( rotate == ROTATE_CCW )
			cornerRotationAngles[ corner ] -= 360.0 / 3.0;
		else
			cornerRotationAngles[ corner ] += 360.0 / 3.0;
	}

	PushMatrix();

	Matrix rotationMatrix;
	CalcMatrixForCorner( corner, rotationMatrix );

	//rotationMatrix.Transpose();

	MulMatrix( rotationMatrix );

	int triCycle[6][3][3] =
	{
		{
			{ 2, 0, 0 }, { 0, 0, 2 }, { 0, 2, 0 }
		},
		{
			{ 2, 0, 1 }, { 0, 1, 2 }, { 1, 2, 0 }
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

	int count = ( rotate == ROTATE_CW ? 2 : 1 );
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