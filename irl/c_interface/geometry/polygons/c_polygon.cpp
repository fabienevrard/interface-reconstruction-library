// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "irl/c_interface/geometry/polygons/c_polygon.h"

extern "C" {

void c_Poly_new(c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr == nullptr);
  a_self->obj_ptr = new IRL::Polygon;
}

void c_Poly_delete(c_Poly* a_self) {
  delete a_self->obj_ptr;
  a_self->obj_ptr = nullptr;
}

void c_Poly_construct(c_Poly* a_self, const int* a_number_of_pts,
                      const double* a_pts) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  assert(*a_number_of_pts >= 0);
  (*a_self->obj_ptr) = IRL::Polygon::fromRawDoublePointer(
      static_cast<IRL::UnsignedIndex_t>(*a_number_of_pts), a_pts);
}

void c_Poly_calculateNormal(const c_Poly* a_self, double* a_normal) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  IRL::Normal face_normal(a_self->obj_ptr->calculateNormal());
  for (IRL::UnsignedIndex_t n = 0; n < 3; ++n) {
    a_normal[n] = face_normal[n];
  }
}

void c_Poly_getLocalizer(const c_Poly* a_self,
                         c_PlanarLoc* a_planar_localizer) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  assert(a_planar_localizer != nullptr);
  assert(a_planar_localizer->obj_ptr != nullptr);

  *a_planar_localizer->obj_ptr = a_self->obj_ptr->getLocalizer();
}

void c_Poly_reversePtOrdering(c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  a_self->obj_ptr->reversePtOrdering();
}

void c_Poly_getBoundingPts(const c_Poly* a_self,
                           double* __restrict__ a_lower_pt,
                           double* __restrict__ a_upper_pt) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  IRL::Pt lower_pt = a_self->obj_ptr->getLowerLimits();
  IRL::Pt upper_pt = a_self->obj_ptr->getUpperLimits();
  for (IRL::UnsignedIndex_t n = 0; n < 3; ++n) {
    a_lower_pt[n] = lower_pt[n];
    a_upper_pt[n] = upper_pt[n];
  }
}

int c_Poly_getNumberOfPts(const c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  return static_cast<int>(a_self->obj_ptr->getNumberOfVertices());
}

void c_Poly_getPt(const c_Poly* a_self, const int* a_index, double* a_pt) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  assert(*a_index >= 0 &&
         *a_index < static_cast<int>(a_self->obj_ptr->getNumberOfVertices()));
  a_pt[0] = (*a_self->obj_ptr)[static_cast<IRL::UnsignedIndex_t>(*a_index)].x();
  a_pt[1] = (*a_self->obj_ptr)[static_cast<IRL::UnsignedIndex_t>(*a_index)].y();
  a_pt[2] = (*a_self->obj_ptr)[static_cast<IRL::UnsignedIndex_t>(*a_index)].z();
}

int c_Poly_getNumberOfSimplicesInDecomposition(c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  return static_cast<int>(
      a_self->obj_ptr->getNumberOfSimplicesInDecomposition());
}

void c_Poly_getSimplexFromDecomposition(c_Poly* a_self,
                                        const int* a_tri_number_to_get,
                                        c_Tri* a_tri_in_decomposition) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  assert(a_tri_in_decomposition != nullptr);
  assert(a_tri_in_decomposition->obj_ptr != nullptr);
  assert(*a_tri_number_to_get >= 0);
  assert(static_cast<IRL::UnsignedIndex_t>(*a_tri_number_to_get) <
         a_self->obj_ptr->getNumberOfSimplicesInDecomposition());
  (*a_tri_in_decomposition->obj_ptr) =
      a_self->obj_ptr->getSimplexFromDecomposition(
          static_cast<IRL::UnsignedIndex_t>(*a_tri_number_to_get));
}

void c_Poly_zeroPolygon(c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  (*a_self->obj_ptr) = IRL::Polygon();
}

void c_Poly_calculateNearestPtOnSurface(const c_Poly* a_self,
                                        const double* a_pt,
                                        double* a_pt_on_polygon) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  auto nearest_pt = a_self->obj_ptr->calculateNearestPtOnSurface(
      IRL::Pt::fromRawDoublePointer(a_pt));
  a_pt_on_polygon[0] = nearest_pt[0];
  a_pt_on_polygon[1] = nearest_pt[1];
  a_pt_on_polygon[2] = nearest_pt[2];
}

double c_Poly_calculateVolume(const c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  return a_self->obj_ptr->calculateVolume();
}

double c_Poly_calculateSign(const c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  return a_self->obj_ptr->calculateSign();
}

void c_Poly_setPlaneOfExistence(c_Poly* a_self, const double* a_plane) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  a_self->obj_ptr->setPlaneOfExistence(
      IRL::Plane(IRL::Normal(a_plane[0], a_plane[1], a_plane[2]), a_plane[3]));
}

void c_Poly_calculateAndSetPlaneOfExistence(c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  a_self->obj_ptr->calculateAndSetPlaneOfExistence();
}

void c_Poly_getPlaneOfExistence(const c_Poly* a_self, double* a_plane) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  const IRL::Plane& plane = a_self->obj_ptr->getPlaneOfExistence();
  a_plane[0] = plane.normal()[0];
  a_plane[1] = plane.normal()[1];
  a_plane[2] = plane.normal()[2];
  a_plane[3] = plane.distance();
}

void c_Poly_calculateCentroid(const c_Poly* a_self, double* a_centroid) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  const IRL::Pt calc_centroid = a_self->obj_ptr->calculateCentroid();
  a_centroid[0] = calc_centroid[0];
  a_centroid[1] = calc_centroid[1];
  a_centroid[2] = calc_centroid[2];
}

void c_Poly_printToScreen(const c_Poly* a_self) {
  assert(a_self != nullptr);
  assert(a_self->obj_ptr != nullptr);
  std::cout << (*a_self->obj_ptr) << std::endl;
}
}
