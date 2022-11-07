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
    constexpr simd::float3 add( const simd::float3& a, const simd::float3& b );
    
    constexpr simd_float4x4 makeIdentity() {
        using simd::float4;
        
        return (simd_float4x4){ (float4){ 1.f, 0.f, 0.f, 0.f },
            (float4){ 0.f, 1.f, 0.f, 0.f },
            (float4){ 0.f, 0.f, 1.f, 0.f },
            (float4){ 0.f, 0.f, 0.f, 1.f } };
    }

    simd::float4x4 makePerspective( float fovRadians, float aspect, float znear, float zfar );
    simd::float4x4 makeXRotate( float angleRadians );
    simd::float4x4 makeYRotate( float angleRadians );
    simd::float4x4 makeZRotate( float angleRadians );
    simd::float4x4 makeTranslate( const simd::float3& v );
    simd::float4x4 makeScale( const simd::float3& v );
    simd::float3x3 discardTranslation( const simd::float4x4& m );
}

#endif /* _DMath_hpp */
