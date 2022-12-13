//
//  3DMath.cpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 06.11.2022.
//

#include "Math3D.hpp"

namespace Math3D {

simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar ) {
    using simd::float4;

    float ys = 1.f / tanf(fovRadians * 0.5f);
    float xs = ys / aspect;
    float zs = zfar / ( znear - zfar );

    return simd_matrix_from_rows((float4){ xs, 0.0f, 0.0f, 0.0f },
                                 (float4){ 0.0f, ys, 0.0f, 0.0f },
                                 (float4){ 0.0f, 0.0f, zs, znear * zs },
                                 (float4){ 0, 0, -1, 0 });
    
//    using simd::float4;
//
//    float xs = aspect * fovRadians;
//    float ys = fovRadians;
//    float zs = zfar / (zfar - znear);
//    float smth1 = ( -zfar * znear ) / (zfar - znear);
//
//    return simd_matrix_from_rows((float4) { xs, 0,  0,      0 },
//                                 (float4) { 0,  ys, 0,      0 },
//                                 (float4) { 0,  0,  zs,     1 },
//                                 (float4) { 0,  0,  smth1,  0 });
}

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

int cantorPairingFunction(int a, int b) {
    return (a + b + 1) * (a + b) / 2 + b;
};

size_t hash3D(int a, int b, int c) {
    return cantorPairingFunction(a, cantorPairingFunction(b, c));
};

}
