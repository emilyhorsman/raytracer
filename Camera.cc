#include <cmath>

#include "Camera.h"
#include "Vector.h"
#include "Utility.h"


Vec3f imagePlaneNormal({ 0, 0, -1 });


Camera::Camera()
: mFieldOfViewRadians(M_PI / 3)
, mPosition{{ 0, 0, 1 }}
, mLookAt{{ 0, 0, -1 }}
, mApertureRadius(0)
{}


void Camera::computePrimaryRay(float pixelX, float pixelY, Vec3f &direction, Vec3f &origin) {
    // Depth of field based on [5][6][7]
    // This is the pre-DOF ray that projects through the pixel in image space.
    Vec3f ray = normalize(Vec3f({ pixelX, pixelY, -1 }));
    float t;
    // There is an imaginary focal plane between the image plane and the
    // objects in the scene. We know one point on the plane: the point the
    // camera is looking at.
    assert(rayPlaneIntersection(
        mPosition,
        ray,
        mLookAt,
        imagePlaneNormal,
        t
    ));
    // Compute the point on the imaginary focal plane that this ray intersects.
    Vec3f focalPoint = add(
        mPosition,
        multiply(ray, t)
    );

    // Project the ray from the aperture point (which sits on the image plane)
    // instead of from the eye. Derive the aperture point from the image point
    // by ``jittering'' the point within a circular aperture.
    //
    // TODO: It'd be cool to have non-circular apertures since they provide
    // different shapes of bokeh.
    Vec3f imagePoint({ pixelX, pixelY, 0 });
    Vec3f aperturePoint = add(
        imagePoint,
        randomDiskPoint(0, mApertureRadius)
    );

    // Compute the actual primary ray!
    direction = normalize(subtract(focalPoint, aperturePoint));
    origin = aperturePoint;
}
