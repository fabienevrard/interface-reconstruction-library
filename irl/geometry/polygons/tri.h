// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef IRL_GEOMETRY_POLYGONS_TRI_H_
#define IRL_GEOMETRY_POLYGONS_TRI_H_

#include <algorithm>
#include <utility>

#include "irl/geometry/general/normal.h"
#include "irl/geometry/general/pt.h"
#include "irl/geometry/general/pt_list.h"
#include "irl/geometry/half_edge_structures/half_edge_polygon.h"
#include "irl/geometry/polygons/general_polygon.h"
#include "irl/geometry/polygons/polygon_storage_types.h"
#include "irl/helpers/byte_buffer.h"
#include "irl/helpers/helper.h"
#include "irl/helpers/mymath.h"
#include "irl/helpers/serializer.h"
#include "irl/moments/volume_moments.h"
#include "irl/moments/volume_moments_and_normal.h"
#include "irl/planar_reconstruction/planar_localizer.h"

namespace IRL {

template <class GeometryType>
class ProxyTri;

template <class Derived, class VertexType>
class TriSpecialization
    : public GeneralPolygon<Derived, VertexType, ProxyTri<Derived>> {
 public:
  using pt_type = VertexType;

  /// \brief Returns the triangle.
  static constexpr UnsignedIndex_t getNumberOfSimplicesInDecomposition(void);

  static constexpr std::array<UnsignedIndex_t, 3>
  getSimplexIndicesFromDecomposition(UnsignedIndex_t a_tri_number_to_get);

  /// \brief Const version of getSimplexFromDecomposition
  ProxyTri<Derived> getSimplexFromDecomposition(
      const UnsignedIndex_t a_tri_number_to_get) const;

  HalfEdgePolygon<pt_type> generateHalfEdgeVersion(void) const;

  template <class HalfEdgePolygonType>
  inline void setHalfEdgeVersion(
      HalfEdgePolygonType* a_half_edge_version) const;

  void reversePtOrdering(void);

  Pt calculateCentroid(void) const;

  Volume calculateVolume(void) const;
  Volume calculateAbsoluteVolume(void) const;
  VolumeMomentsAndNormal calculateVolumeMomentsAndNormal(void) const;
  VolumeMoments calculateMoments(void) const;
  double calculateSign(void) const;
};

template <class GeometryType>
class ProxyTri
    : public ProxyStaticStorageAndRefferredToPlane<ProxyTri<GeometryType>,
                                                   GeometryType, 3>,
      public TriSpecialization<ProxyTri<GeometryType>,
                               typename GeometryType::pt_type> {
  friend ProxyStaticStorageAndRefferredToPlane<ProxyTri<GeometryType>,
                                               GeometryType, 3>;

 public:
  using ProxyStaticStorageAndRefferredToPlane<
      ProxyTri<GeometryType>, GeometryType,
      3>::ProxyStaticStorageAndRefferredToPlane;
};

template <class VertexType>
class StoredTri
    : public StaticStorageAndStoredPlane<StoredTri<VertexType>, VertexType, 3>,
      public TriSpecialization<StoredTri<VertexType>, VertexType> {
  friend StaticStorageAndStoredPlane<StoredTri<VertexType>, VertexType, 3>;

 public:
  using StaticStorageAndStoredPlane<StoredTri<VertexType>, VertexType,
                                    3>::StaticStorageAndStoredPlane;

  StoredTri(void) = default;

  template <class GeometryType>
  explicit StoredTri(const ProxyTri<GeometryType>& a_tri_proxy);

  template <class GeometryType>
  StoredTri& operator=(const ProxyTri<GeometryType>& a_tri_proxy);
};

///////////////////////////////////////////
// Predefined types
using Tri = StoredTri<Pt>;

}  // namespace IRL

#include "irl/geometry/polygons/tri.tpp"

#endif // IRL_GEOMETRY_POLYGONS_TRI_H_
