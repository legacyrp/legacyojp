#include "CVec.h"

CVec3::CVec3( const vec3_t a )
{//constructor by vec3_t
	data[0] = a[0];
	data[1] = a[1];
	data[2] = a[2];
	data[3] = 1;
}


float & CVec3::operator[](int i)
{//subscript operator
	switch(i)
	{
	case 1:
		return x;
		break;
	case 2:
		return y;
		break;
	case 3:
		return z;
		break;
	default:
		G_Printf("Index %i out of range in CVec3\n", i);
		return x;
		break;
	};
}


float CVec3::operator[](int i) const
{//const subscript operator
	switch(i)
	{
	case 1:
		return x;
		break;
	case 2:
		return y;
		break;
	case 3:
		return z;
		break;
	default:
		G_Printf("Index %i out of range in CVec3\n", i);
		return x;
		break;
	};
}


CVec3& CVec3::operator = (const CVec3& b)
{//assignment
	x = b.x;
	y = b.y;
	z = b.z;
	return *this;
}


CVec3 operator + (const CVec3& a, const CVec3& b)
{//addition by vector
	CVec3 temp;
	temp.data[0] = a.data[0] + b.data[0];
	temp.data[1] = a.data[1] + b.data[1];
	temp.data[2] = a.data[2] + b.data[2];
	temp.data[3] = a.data[3] + b.data[3];
	return temp;
}


CVec3 operator - (const CVec3& a, const CVec3& b)
{//subtraction by vector
	CVec3 temp;
	temp.data[0] = a.data[0] - b.data[0];
	temp.data[1] = a.data[1] - b.data[1];
	temp.data[2] = a.data[2] - b.data[2];
	temp.data[3] = a.data[3] - b.data[3];
	return temp;
}


CVec3& CVec3::operator *= (CVec3 b)
{
	data[0] *= b.data[0];
	data[1] *= b.data[1];
	data[2] *= b.data[2];
	data[3] *= b.data[3];
	return *this;
}


CVec3& CVec3::operator *= (float b)
{
	data[0] *= b;
	data[1] *= b;
	data[2] *= b;
	data[3] *= b;
	return *this;
}


CVec3& CVec3::operator += (CVec3 b)
{
	data[0] += b.data[0];
	data[1] += b.data[1];
	data[2] += b.data[2];
	data[3] += b.data[3];
	return *this;
}


CVec3 operator* ( CVec3 a, float b )
{
	CVec3 temp;
	temp.data[0] = a.data[0] * b;
	temp.data[1] = a.data[1] * b;
	temp.data[2] = a.data[2] * b;
	temp.data[3] = a.data[3] * b;
	return temp;
}


CVec3& CVec3::operator -= (CVec3 b)
{//-= by vector
	x -= b.x;
	y -= b.y;
	z -= b.z;
	return *this;
}


bool CVec3::operator != (vec3_t b)
{
	if( x == b[0] &&
		y == b[1] &&
		z == b[2] )
	{//they're ==
		return false;
	}
	return true;
}


CVec3& CVec3::operator /= (float b)
{
	x /= b;
	y /= b;
	z /= b;
	return *this;
}


bool CVec3::operator < (CVec3 b)
{
	if( x < b.x &&
		y < b.y &&
		z < b.z )
	{
		return true;
	}
	return false;
}


bool CVec3::operator > (CVec3 b)
{
	if( x > b.x &&
		y > b.y &&
		z > b.z )
	{
		return true;
	}
	return false;
}




//Functions!
//--------------

float ProjectPointOntoVector( const vec3_t point, const vec3_t vStart, const vec3_t vEnd, vec3_t vProj )
{//this helper function projects a point onto a given line with const inputs.
	//returns the dot product between the line vector and the point in relation to the line.  
	//IE any result between 0-1 means the point projected into the line inbetween the line points.
	vec3_t pVec, vec;
	float linePointDot;

	VectorSubtract( point, vStart, pVec );
	VectorSubtract( vEnd, vStart, vec );

	//figure out the line vector/point dot product.
	linePointDot = DotProduct( pVec, vec );

	VectorNormalize( vec );
	// project onto the directional vector for this segment
	VectorMA( vStart, DotProduct( pVec, vec ), vec, vProj );

	return linePointDot;
}


float CVec3::Dist( CVec3 b ) const
{//gives the distance between this point and the inputted point.
	return Distance(v, b.v);
}


float CVec3::Dist2( CVec3 b ) const
{//gives the squared distance between this point and the inputted point.
	return DistanceSquared(v, b.v);
}



float CVec3::DistToLine( CVec3 a, CVec3 b ) const
{//gives the distance between this point and the line generated by the two inputted points.
	vec3_t vec, p2;

	ProjectPointOntoVector(v, a.v, b.v, p2);
	VectorSubtract(v, p2, vec);
	return VectorLength(vec);
}



float CVec3::DistToLine2( CVec3 a, CVec3 b ) const
{//gives the squared distance between this point and the line generated by the two inputted points.
	vec3_t p2;

	ProjectPointOntoVector(v, a.v, b.v, p2);
	return DistanceSquared(v, p2);
}


void CVec3::ToStr(char * dest)
{//converts the vector info into a string char.
	strcpy(dest, va("%f %f %f", x, y, z));
}



float CVec3::SafeNorm()
{	//"safe" normalization of the vector. 
	//normalizes vector and returns original vector length.  
	//RAFIXME - I assume the safe normalization is to prevent zero vectors from breaking stuff.  Need it?
	return VectorNormalize(v);
}


	
float CVec3::Norm()
{//normalize the vector
	return VectorNormalize(v);
}


float CVec3::ProjectToLine( const CVec3 a, const CVec3 b )
{//I assume this means returning the closest point to this vector from the line formed by points a and b.
//returns the dot product between the line vector and the point in relation to the line.  
//IE any result between 0-1 means the point projected into the line inbetween the line points.
	return ProjectPointOntoVector(v, a.v, b.v, v);
}


float CVec3::ProjectToLineSeg( const CVec3 a, const CVec3 b )
{//project this vector onto the line segment formed by these points.  
	//returns the dot product between the line vector and the point in relation to the line.  
//IE any result between 0-1 means the point projected into the line inbetween the line points.
	//project point / get dot
	float dot = ProjectPointOntoVector(v, a.v, b.v, v);

	if( dot < 0 )
	{//projected beyond start point of line (a), capping it
		VectorCopy(a.v, v);
	}
	else if( dot > 1.0f )
	{//projected beyond end point of line (b), capping it
		VectorCopy(b.v, v);
	}

	return dot;
}


float CVec3::Dot(const CVec3 a)
{//do the dot product for these two points.
	return DotProduct(v, a.v);
}



float CVec3::Len()
{//returns the length of this vector
	return VectorLength(v);
}


float CVec3::Len2()
{//returns the squared length of this vector
	return VectorLengthSquared(v);
}


void CVec3::Clear()
{//clears the vector
	x = 0;
	y = 0;
	z = 0;
}


bool CVec3::IsFinite()
{//checks to see if this vector is zero?
	if( x != 0 ||
		y != 0 ||
		z != 0 )
	{
		return false;
	}

	return true;
}


	
float CVec3::Truncate( float trunValue )
{//truncate vector length to this value
	float length = VectorLength(v);

	if(trunValue < 0)
	{//can't truncate to negative values!
		G_Printf("negative value %f given as truncation value for CVec3.Truncate()\n", trunValue);
		return length;
	}

	if(length > trunValue)
	{//truncating.
		VectorNormalize(v);
		VectorScale(v, trunValue, v);
	}

	//no truncation needed, just return vector length
	return length;
}

	
void CVec3::VecToAng()
{//converts the vector data to angle data.
	vec3_t temp;
	vectoangles(v, temp);
	x = temp[0];
	y = temp[1];
	z = temp[2];
}


void CVec3::AngToVec()
{//converts the angle data back to vector data
	vec3_t fwd;
	AngleVectors(v, fwd, NULL, NULL);
	x = fwd[0];
	y = fwd[1];
	z = fwd[2];
}



void CVec3::ScaleAdd(CVec3 dir, float scalar)
{//I think this is a VectorMA thingy
	VectorMA(v, scalar, dir, v);
}


float& CVec3::pitch()
{//PITCH = x
	return x;
}


void CVec3::Cross(float a, float b, float c)
{//cross product
	vec3_t temp, result;

	//construct crossing vector.
	temp[0] = a;
	temp[1] = b;
	temp[2] = c;

	//cross and place result into CVec3
	CrossProduct(v, temp, result);
	VectorCopy(result, v);
}





