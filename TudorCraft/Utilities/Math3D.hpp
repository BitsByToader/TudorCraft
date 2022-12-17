//
//  3DMath.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 06.11.2022.
//

#ifndef _DMath_hpp
#define _DMath_hpp

#include <simd/simd.h>

namespace Math3D {
//MARK: - 3D -> 2D Matrixes and others
simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar );
simd::float4x4 makePointAt(simd::float3 position, simd::float3 target, simd::float3 up);
simd::float4x4 makeTranslate( const simd::float3& v );
simd::float4x4 makeScale( const simd::float3& v );
simd::float3x3 discardTranslation( const simd::float4x4& m );

//MARK: - 4x4 Rotation Matrixes
simd::float4x4 makeXRotate4x4( float angleRadians );
simd::float4x4 makeYRotate4x4( float angleRadians );
simd::float4x4 makeZRotate4x4( float angleRadians );

//MARK: - 3x3 Rotation Matrixes
simd::float3x3 makeXRotate3x3( float angleRadians );
simd::float3x3 makeYRotate3x3( float angleRadians );
simd::float3x3 makeZRotate3x3( float angleRadians );

// MARK: - Transformation matrixes for mesh creation
inline const simd::float4x4 moveFaceToBack() {
    return makeYRotate4x4(M_PI);
};

inline const simd::float4x4 moveFaceToRight() {
    return makeYRotate4x4(M_PI/2);
};

inline const simd::float4x4 moveFaceToLeft() {
    return makeYRotate4x4(3 * M_PI/2);
};

inline const simd::float4x4 moveFaceToTop() {
    return makeXRotate4x4(M_PI/2);
};

inline const simd::float4x4 moveFaceToBottom() {
    return makeXRotate4x4(3 * M_PI/2);
};

// MARK: - Hashing (pairing) of 3D coordinates
size_t hash3D(int a,  int b, int c);

}

#endif /* _DMath_hpp */
