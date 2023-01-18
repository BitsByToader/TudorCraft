//
//  Faces.hpp
//  TudorCraft
//
//  Created by Tudor Ifrim on 29.12.2022.
//

#ifndef Faces_h
#define Faces_h

/// Maps each face of a cuboid to an index in an array with 6 elements.
///
/// Used in multiple places, e.g: when generating the mesh, when deciding on which face to place a block,
/// when checking for collisions etc.
enum Faces: unsigned char {
    Front   = 0,
    Left    = 1,
    Back    = 2,
    Right   = 3,
    Bottom  = 4,
    Top     = 5
};

#endif /* Faces_h */
