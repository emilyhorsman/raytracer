#include <cmath>

#include "Camera.h"
#include "Vector.h"


Camera::Camera()
: mFieldOfViewRadians(M_PI / 2.2f)
, mPosition{{ 0, 0, 0.25 }}
, mLookAt{{ 0.5, 0.8, -2 }}
, mApertureRadius(0)
{}
