#ifndef __ndXfrms_h
#define __ndXfrms_h

#include "vtkTrackerWidget_global.h"

#define BAD_FLOAT    (float)-3.697314E28
#define MAX_NEGATIVE (float)-3.0E28

#define isValidFloat( x )      ( x > MAX_NEGATIVE )

typedef struct Position3dStruct
{
    float   x;
    float   y;
    float   z;
} Position3d;

typedef struct QuatRotationStruct
{
    float   q0;
    float   qx;
    float   qy;
    float   qz;
} QuatRotation;

typedef struct QuatTransformationStruct
{
    QuatRotation     rotation;
    Position3d       translation;
} QuatTransformation;

void VTKTRACKERWIDGET_EXPORT SetXfrmMissing( QuatTransformation *xfrm);
int VTKTRACKERWIDGET_EXPORT IsQuatRotationMissing( QuatRotation *rotation);
int VTKTRACKERWIDGET_EXPORT IsPosition3dMissing( Position3d *position);
int VTKTRACKERWIDGET_EXPORT IsXfrmMissing( QuatTransformation *xfrm);
void VTKTRACKERWIDGET_EXPORT CopyTransform( QuatTransformation *fromXfrm, 
                   QuatTransformation *toXfrm);

#endif