#include "ndXfrms.h"

void SetXfrmMissing(QuatTransformation *xfrm)
{
  xfrm->rotation.q0 = BAD_FLOAT;
  xfrm->rotation.qx = BAD_FLOAT;
  xfrm->rotation.qy = BAD_FLOAT;
  xfrm->rotation.qz = BAD_FLOAT;
  xfrm->translation.x = BAD_FLOAT;
  xfrm->translation.y = BAD_FLOAT;
  xfrm->translation.z = BAD_FLOAT;
}

int IsQuatRotationMissing( QuatRotation *rotation)
{
  if( !isValidFloat(rotation->q0) || 
    !isValidFloat(rotation->qx) || 
    !isValidFloat(rotation->qy) || 
    !isValidFloat(rotation->qz) )
  {
    return 1;
  }

  return 0;
}

int IsPosition3dMissing( Position3d *position)
{
  if( !isValidFloat( position->x ) ||
    !isValidFloat( position->y ) ||
    !isValidFloat( position->z ) )
  {
    return 1;
  }

  return 0;
}

int IsXfrmMissing(QuatTransformation *xfrm)
{
  if( IsQuatRotationMissing( &xfrm->rotation ) )
    return 1;

  if( IsPosition3dMissing(&xfrm->translation) ) 
    return 1;

  return 0;
}

void CopyTransform(QuatTransformation *fromXfrm, QuatTransformation *toXfrm)
{
  toXfrm->rotation.q0 = fromXfrm->rotation.q0;
  toXfrm->rotation.qx = fromXfrm->rotation.qx;
  toXfrm->rotation.qy = fromXfrm->rotation.qy;
  toXfrm->rotation.qz = fromXfrm->rotation.qz;
  toXfrm->translation.x = fromXfrm->translation.x;
  toXfrm->translation.y = fromXfrm->translation.y;
  toXfrm->translation.z = fromXfrm->translation.z;
}