// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef IRL_GENERIC_CUTTING_SIMPLEX_CUTTING_SIMPLEX_CUTTING_DRIVERS_H_
#define IRL_GENERIC_CUTTING_SIMPLEX_CUTTING_SIMPLEX_CUTTING_DRIVERS_H_

#include "irl/generic_cutting/general/class_classifications.h"
#include "irl/helpers/SFINAE_boiler_plate.h"

namespace IRL {

// Main driving function that is specialized through SFINAE
template <class SegmentedPolytopeType, class HalfEdgePolytopeType,
          class ReconstructionType, class ReturnType>
void getVolumeMomentsForDecomposedPolytope(
    SegmentedPolytopeType* a_polytope,
    HalfEdgePolytopeType* a_complete_polytope,
    const ReconstructionType& a_reconstruction,
    ReturnType* a_moments_to_return);

}  // namespace IRL

#include "irl/generic_cutting/simplex_cutting/simplex_cutting_drivers.tpp"

#endif // IRL_GENERIC_CUTTING_SIMPLEX_CUTTING_SIMPLEX_CUTTING_DRIVERS_H_
