/**
 @filename Utility.h
 
 
*/
#pragma once

#include "../genoype/GenotypeParser.h"


/**
 @brief 
 @date 2014-02-10
*/
inline void quatRotationArc(PxQuat &q, const PxVec3& v0, const PxVec3& v1)
{
	PxVec3 c = v1.cross(v0);
	if (c.isZero())
	{
		q = PxQuat(0,PxVec3(0,0,0));
		return;
	}

	double s = sqrtf((1.0f+v0.dot(v1))*2.0f);
	q.x = c.x/s;
	q.y = c.y/s;
	q.z = c.z/s;
	q.w = s/2.0f;
}


inline const PxVec3 Vec3toPxVec3(const evc::genotype_parser::SVec3 &rhs)
{
	return PxVec3(rhs.x, rhs.y, rhs.z);
}


inline void convertD3D9(D3DMATRIX &dxmat, const physx::PxMat44 &mat)
{
	//PxMat44 mat44 = mat.getTranspose();
	//memcpy(&dxmat._11, mat44.front(), 4 * 4 * sizeof (float));
	memcpy(&dxmat._11, mat.front(), 4 * 4 * sizeof (float));
}


namespace SampleRenderer { class RendererProjection; }
void convertD3D9(D3DMATRIX &dxmat, const SampleRenderer::RendererProjection &mat);

