//
//  3DMath.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 06.11.2022.
//

#include "Math3D.hpp"

namespace Math3D {


constexpr simd::float3 add( const simd::float3& a, const simd::float3& b ) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar ) {
    using simd::float4;
    
    float ys = 1.f / tanf(fovRadians * 0.5f);
    float xs = ys / aspect;
    float zs = zfar / ( znear - zfar );
    
    return simd_matrix_from_rows((float4){ xs, 0.0f, 0.0f, 0.0f },
                                 (float4){ 0.0f, ys, 0.0f, 0.0f },
                                 (float4){ 0.0f, 0.0f, zs, znear * zs },
                                 (float4){ 0, 0, -1, 0 });
}

// TODO: Investigate if this is faster than just making a basic rotation matrix with sin and cos functions
// This function basically rotates the world from initial position to a target vector
// Similarly, for rotation and scaling matrixes, there is a specific way an inverse looks like
// It might be faster than using simd's inverse, but it's a pain to write and uses three dot products on the last line...
simd::float4x4 makePointAt(simd::float3 position, simd::float3 target, simd::float3 up) {
    // Calculate new forward vector
    simd::float3 newForward = target - position;
    newForward = simd_normalize(newForward);
    
    // Calculate new Up vector
    simd::float3 a = newForward * simd_dot(up, newForward);
    simd::float3 newUp = up - a;
    newUp = simd_normalize(newUp);
    
    // Calculate new right direction
    simd::float3 newRight= simd_cross(newUp, newForward);
    
    return simd_matrix_from_rows((simd::float4){newRight.x,   newRight.y,   newRight.z,   0},
                                 (simd::float4){newUp.x,      newUp.y,      newUp.z,      0},
                                 (simd::float4){newForward.x, newForward.y, newForward.z, 0},
                                 (simd::float4){position.x,   position.y,   position.z,    1});
};

simd::float4x4 makeXRotate4x4( float angleRadians ) {
    using simd::float4;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float4){ 1.0f, 0.0f, 0.0f, 0.0f },
                                 (float4){ 0.0f, cosf( a ), sinf( a ), 0.0f },
                                 (float4){ 0.0f, -sinf( a ), cosf( a ), 0.0f },
                                 (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
}

simd::float4x4 makeYRotate4x4( float angleRadians ) {
    using simd::float4;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float4){ cosf( a ), 0.0f, sinf( a ), 0.0f },
                                 (float4){ 0.0f, 1.0f, 0.0f, 0.0f },
                                 (float4){ -sinf( a ), 0.0f, cosf( a ), 0.0f },
                                 (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
}

simd::float4x4 makeZRotate4x4( float angleRadians ) {
    using simd::float4;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float4){ cosf( a ), sinf( a ), 0.0f, 0.0f },
                                 (float4){ -sinf( a ), cosf( a ), 0.0f, 0.0f },
                                 (float4){ 0.0f, 0.0f, 1.0f, 0.0f },
                                 (float4){ 0.0f, 0.0f, 0.0f, 1.0f });
}

simd::float3x3 makeXRotate3x3( float angleRadians ) {
    using simd::float3;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float3){ 1.0f, 0.0f,    0.0f     },
                                 (float3){ 0.0f, cosf(a), -sinf(a) },
                                 (float3){ 0.0f, sinf(a), cosf(a)  });
};

simd::float3x3 makeYRotate3x3( float angleRadians ) {
    using simd::float3;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float3){ cosf(a),  0.0f, sinf(a) },
                                 (float3){ 0.0f,     1.0f, 0.0f    },
                                 (float3){ -sinf(a), 0.0f, cosf(a) });
};

simd::float3x3 makeZRotate3x3( float angleRadians ) {
    using simd::float3;
    
    const float a = angleRadians;
    return simd_matrix_from_rows((float3){ cosf(a), -sinf(a), 0.0f },
                                 (float3){ sinf(a), cosf(a),  0.0f },
                                 (float3){ 0.0f,    0.0f,     1.0f });
};

simd::float4x4 makeTranslate( const simd::float3& v ) {
    using simd::float4;
    
    const float4 col0 = { 1.0f, 0.0f, 0.0f, 0.0f };
    const float4 col1 = { 0.0f, 1.0f, 0.0f, 0.0f };
    const float4 col2 = { 0.0f, 0.0f, 1.0f, 0.0f };
    const float4 col3 = { v.x, v.y, v.z, 1.0f };
    return simd_matrix( col0, col1, col2, col3 );
}

simd::float4x4 makeScale( const simd::float3& v ) {
    using simd::float4;
    
    return simd_matrix((float4){ v.x, 0, 0, 0 },
                       (float4){ 0, v.y, 0, 0 },
                       (float4){ 0, 0, v.z, 0 },
                       (float4){ 0, 0, 0, 1.0 });
}

simd::float3x3 discardTranslation( const simd::float4x4& m ) {
    return simd_matrix( m.columns[0].xyz, m.columns[1].xyz, m.columns[2].xyz );
}
}
