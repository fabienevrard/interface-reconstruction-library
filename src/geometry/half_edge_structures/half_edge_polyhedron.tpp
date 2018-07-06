// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef SRC_GEOMETRY_HALF_EDGE_STRUCTURES_HALF_EDGE_POLYHEDRON_TPP_
#define SRC_GEOMETRY_HALF_EDGE_STRUCTURES_HALF_EDGE_POLYHEDRON_TPP_

namespace IRL {

template <class PtType, class VertexType, class HalfEdgeType, class FaceType,
          UnsignedIndex_t kMaxHalfEdges, UnsignedIndex_t kMaxVertices,
          UnsignedIndex_t kMaxFaces>
SegmentedHalfEdgePolyhedron<FaceType, VertexType>
HalfEdgePolyhedron<PtType, VertexType, HalfEdgeType, FaceType, kMaxHalfEdges,
                   kMaxVertices, kMaxFaces>::generateSegmentedPolyhedron(void) {
  SegmentedHalfEdgePolyhedron<FaceType, VertexType> a_polyhedron;
  this->setSegmentedPolyhedron(&a_polyhedron);
  return a_polyhedron;
}

template <class PtType, class VertexType, class HalfEdgeType, class FaceType,
          UnsignedIndex_t kMaxHalfEdges, UnsignedIndex_t kMaxVertices,
          UnsignedIndex_t kMaxFaces>
template <class SegmentedType>
void HalfEdgePolyhedron<
    PtType, VertexType, HalfEdgeType, FaceType, kMaxHalfEdges, kMaxVertices,
    kMaxFaces>::setSegmentedPolyhedron(SegmentedType* a_polytope) {
  a_polytope->setNumberOfFaces(this->getNumberOfFaces());
  auto begin_segmented_face_iter = &(a_polytope->getFacePointer(0));
  for (UnsignedIndex_t n = 0; n < this->getNumberOfFaces(); ++n) {
    *begin_segmented_face_iter = &(this->getFace(n));
    ++begin_segmented_face_iter;
  }

  a_polytope->setNumberOfVertices(this->getNumberOfVertices());
  auto begin_segmented_vertex_iter = &(a_polytope->getVertexPointer(0));
  for (UnsignedIndex_t n = 0; n < this->getNumberOfVertices(); ++n) {
    *begin_segmented_vertex_iter = &(this->getVertex(n));
    ++begin_segmented_vertex_iter;
  }
}

}  // namespace IRL

#endif  // SRC_GEOMETRY_HALF_EDGE_STRUCTURES_HALF_EDGE_POLYHEDRON_TPP_