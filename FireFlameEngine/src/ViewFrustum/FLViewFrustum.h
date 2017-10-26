#pragma once
#include <iostream>
#include <math.h>
#include <DirectXMath.h>

namespace FireFlame {
struct stViewFrustum {
	stViewFrustum() = default;
	float l = 0.f, r = 0.f, t = 0.f, b = 0.f, n = 0.f, f = 0.f;
};
struct stViewFrustum8Corner {
	/* LH coordinate system.
	   Looking at +Z axis, array entry index.Just bored at meeting......
	                    ^ Y axis
	                    |
			5-----------|----------6
			| *         |         *|
			|   *       |       *  |
			|     *     |     *    |
			|       1---|---2      |                
	--------|-------|---|---|------|----------------> X axis
			|	    0---|---3      |
			|     *     |     *    |
			|   *       |       *  |
			| *         |         *|
			4-----------|----------7
			            |
						|
	*/
	DirectX::XMFLOAT3 corner[8];
	enum {
		Near_left_bottom = 0,
		Near_left_top,
		Near_right_top,
		Near_right_bottom,
		Far_left_bottom,
		Far_left_top,
		Far_right_top,
		Far_right_bottom
	};
};
inline
stViewFrustum GetViewFrustum(float fovY, float aspectRatio, float n, float f) {
	stViewFrustum frustum;
	float hdiv2 = n * std::tanf(fovY / 2.f);
	float wdiv2 = aspectRatio * hdiv2;
	frustum.l = -wdiv2; frustum.r = wdiv2;
	frustum.t = hdiv2; frustum.b = -hdiv2;
	frustum.n = n; frustum.f = f;
	return frustum;
}
inline 
void FillFrustumCorner(stViewFrustum8Corner& frustum, size_t offset, float dist, float tanFovDiv2, float r) {
	using DirectX::XMFLOAT3;
	float DistTanFovdiv2 = dist*tanFovDiv2;
	frustum.corner[offset]   = XMFLOAT3(-r*DistTanFovdiv2, -DistTanFovdiv2, dist);
	frustum.corner[offset+1] = XMFLOAT3(-r*DistTanFovdiv2,  DistTanFovdiv2, dist);
	frustum.corner[offset+2] = XMFLOAT3( r*DistTanFovdiv2,  DistTanFovdiv2, dist);
	frustum.corner[offset+3] = XMFLOAT3( r*DistTanFovdiv2, -DistTanFovdiv2, dist);
}
inline
stViewFrustum8Corner GetViewFrustum8Corner(float fovY, float aspectRatio, float n, float f) {
	stViewFrustum8Corner frustum;
	float tanFovDiv2 = std::tanf(fovY / 2.f);
	// near plane 4 corners
	FillFrustumCorner(frustum, stViewFrustum8Corner::Near_left_bottom, n, tanFovDiv2, aspectRatio);
	// far plane 4 corners
	FillFrustumCorner(frustum, stViewFrustum8Corner::Far_left_bottom, f, tanFovDiv2, aspectRatio);
	return frustum;
}
inline
std::ostream& operator<<(std::ostream& os, const stViewFrustum& frustum) {
	os << "left:\t" << frustum.l << "\tright:\t" << frustum.r << '\n'
	   << "top:\t" << frustum.t << "\tbottom:\t" << frustum.b << '\n'
	   << "near plane:\t" << frustum.n << "\tfar plane:\t" << frustum.f;
	return os;
}
inline
std::ostream& operator<<(std::ostream& os, const DirectX::XMFLOAT3& v) {
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}
std::ostream& operator<<(std::ostream& os, const stViewFrustum8Corner& frustum) {
	os << "near left bottom:" << frustum.corner[0] << " near left top:" << frustum.corner[1] << '\n'
	   << "near right top:" << frustum.corner[2] << " near right bottom:" << frustum.corner[3] << '\n'
	   << "far right top:" << frustum.corner[4] << " far right bottom:" << frustum.corner[5] << '\n'
	   << "far right top:" << frustum.corner[6] << " far right bottom:" << frustum.corner[7];
	return os;
}
}