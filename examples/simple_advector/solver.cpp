// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <stdio.h>
#include <cmath>
#include <cstdio>
#include <string>

#include "irl/generic_cutting/generic_cutting_definitions.h"
#include "irl/geometry/polyhedrons/dodecahedron.h"
#include "irl/geometry/polyhedrons/rectangular_cuboid.h"
#include "irl/interface_reconstruction_methods/elvira_neighborhood.h"
#include "irl/interface_reconstruction_methods/reconstruction_interface.h"

#include "examples/simple_advector/diagnostics.h"
#include "examples/simple_advector/solver.h"

void runSimulation(const int a_number_of_cells, const int a_number_of_rotations,
                   const int a_viz_out_freq) {
  // Initialize mesh and interface/cell/traversal reconstructions
  auto mesh = initializeMesh(a_number_of_cells);
  IRLReconstructionsPack reconstructions;
  reconstructions.liquid_gas_interface = initializeInterface(mesh);
  reconstructions.cell_localizers = initializeCellLocalizers(mesh);
  reconstructions.traversing_links = setupLinkStructure(
      reconstructions.liquid_gas_interface, reconstructions.cell_localizers);

  // Calculate volume field from set planar reconstructions
  auto liquid_volume_fraction =
      initializeVolumeFraction(reconstructions.liquid_gas_interface);
  auto initial_liquid_volume_fraction = liquid_volume_fraction;

  // Calculate constant timestep for CFL = 0.5 and number of steps per
  // revolution at this time. Also make sure it is multiple of
  // a_viz_out_freq for ease of visualization output.
  constexpr double max_vel =
      std::sqrt(0.5 * 0.5 + 0.5 * 0.5) * M_PI * std::sqrt(2.0);
  constexpr double desired_CFL = 0.5;
  double dt = desired_CFL * mesh.dx() / max_vel;
  int steps_per_revolution = static_cast<int>(std::floor(1.0 / dt));
  if (a_viz_out_freq > 0) {
    steps_per_revolution =
        steps_per_revolution + steps_per_revolution % a_viz_out_freq;
  }
  dt = 1.0 / steps_per_revolution;

  // Initialize folders/mesh for very simple I/O
  int viz_output = 0;
  writeOutMesh(mesh);

  // Now carry out the forward solution, doing it based on
  // the number of revolutions requested and progress
  // per revolution.
  int iteration = 0;
  for (int revolutions = 0; revolutions < a_number_of_rotations;
       ++revolutions) {
    for (int step = 0; step < steps_per_revolution; ++step) {
      advanceState(dt, &liquid_volume_fraction, &reconstructions);
      writeOutDiagnostics(iteration, revolutions, step + 1,
                          steps_per_revolution);
      if (a_viz_out_freq > 0 &&
          step % (steps_per_revolution / a_viz_out_freq) == 0) {
        writeOutVisualization(viz_output, liquid_volume_fraction);
        ++viz_output;
      }
      ++iteration;
    }
    newlineDiagnostic();
  }
  if (a_viz_out_freq > 0) {
    writeOutVisualization(viz_output, liquid_volume_fraction);
  }
}

BasicMesh initializeMesh(const int a_number_of_cells) {
  constexpr const int a_number_of_ghost_cells = 2;
  const double dx = 1.0 / static_cast<double>(a_number_of_cells);
  IRL::Pt lower_domain(-0.5, -0.5, -0.5 * dx);
  IRL::Pt upper_domain(0.5, 0.5, 0.5 * dx);
  BasicMesh mesh(a_number_of_cells, a_number_of_cells, 1,
                 a_number_of_ghost_cells);
  mesh.setCellBoundaries(lower_domain, upper_domain);
  return mesh;
}

Data<IRL::PlanarSeparator> initializeInterface(const BasicMesh& a_mesh) {
  constexpr double circle_radius = 0.1;
  constexpr IRL::Pt circle_center(0.0, 0.25, 0.0);

  Data<IRL::PlanarSeparator> liquid_gas_interface(a_mesh);
  for (int i = a_mesh.imino(); i <= a_mesh.imaxo(); ++i) {
    for (int j = a_mesh.jmino(); j <= a_mesh.jmaxo(); ++j) {
      for (int k = a_mesh.kmino(); k <= a_mesh.kmaxo(); ++k) {
        const IRL::Pt lower_cell_pt(a_mesh.x(i), a_mesh.y(j), a_mesh.z(k));
        const IRL::Pt upper_cell_pt(a_mesh.x(i + 1), a_mesh.y(j + 1),
                                    a_mesh.z(k + 1));
        const IRL::Pt mid_pt = 0.5 * (lower_cell_pt + upper_cell_pt);
        const auto circle_normal =
            IRL::Normal::fromPtNormalized(mid_pt - circle_center);
        liquid_gas_interface(i, j, k) =
            IRL::PlanarSeparator::fromOnePlane(IRL::Plane(
                circle_normal, circle_radius + circle_normal * circle_center));
      }
    }
  }
  return liquid_gas_interface;
}

Data<IRL::PlanarLocalizer> initializeCellLocalizers(const BasicMesh& a_mesh) {
  Data<IRL::PlanarLocalizer> cell_localizers(a_mesh);
  std::array<IRL::Normal, 6> plane_normals;
  plane_normals[0] = IRL::Normal(-1.0, 0.0, 0.0);
  plane_normals[1] = IRL::Normal(1.0, 0.0, 0.0);
  plane_normals[2] = IRL::Normal(0.0, -1.0, 0.0);
  plane_normals[3] = IRL::Normal(0.0, 1.0, 0.0);
  plane_normals[4] = IRL::Normal(0.0, 0.0, -1.0);
  plane_normals[5] = IRL::Normal(0.0, 0.0, 1.0);
  for (int i = a_mesh.imino(); i <= a_mesh.imaxo(); ++i) {
    for (int j = a_mesh.jmino(); j <= a_mesh.jmaxo(); ++j) {
      for (int k = a_mesh.kmino(); k <= a_mesh.kmaxo(); ++k) {
        const IRL::Pt lower_cell_pt(a_mesh.x(i), a_mesh.y(j), a_mesh.z(k));
        const IRL::Pt upper_cell_pt(a_mesh.x(i + 1), a_mesh.y(j + 1),
                                    a_mesh.z(k + 1));
        cell_localizers(i, j, k).setNumberOfPlanes(6);
        cell_localizers(i, j, k)[0] =
            IRL::Plane(plane_normals[0], -a_mesh.x(i));
        cell_localizers(i, j, k)[1] =
            IRL::Plane(plane_normals[1], a_mesh.x(i + 1));
        cell_localizers(i, j, k)[2] =
            IRL::Plane(plane_normals[2], -a_mesh.y(j));
        cell_localizers(i, j, k)[3] =
            IRL::Plane(plane_normals[3], a_mesh.y(j + 1));
        cell_localizers(i, j, k)[4] =
            IRL::Plane(plane_normals[4], -a_mesh.z(k));
        cell_localizers(i, j, k)[5] =
            IRL::Plane(plane_normals[5], a_mesh.z(k + 1));
      }
    }
  }
  return cell_localizers;
}

Data<IRL::LocalizedSeparatorLink> setupLinkStructure(
    const Data<IRL::PlanarSeparator>& a_interface,
    const Data<IRL::PlanarLocalizer>& a_cell_localizers) {
  const BasicMesh& mesh = a_interface.getMesh();
  const int nghost = mesh.getNgc();
  Data<IRL::LocalizedSeparatorLink> traversing_links(mesh);
  // Depends on order of planes in a_cell_localizers. We set up the
  // planes so we inherently know the orders
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        traversing_links(i, j, k) = IRL::LocalizedSeparatorLink(
            &(a_cell_localizers(i, j, k)), &(a_interface(i, j, k)));
        IRL::UnsignedIndex_t tag = static_cast<IRL::UnsignedIndex_t>(
            (i + nghost) + (j + nghost) * mesh.getNxo() +
            (k + nghost) * mesh.getNxo() * mesh.getNyo());
        traversing_links(i, j, k).setId(tag);
        if (i != mesh.imino()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              0, &(traversing_links(i - 1, j, k)));
        }
        if (i != mesh.imaxo()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              1, &(traversing_links(i + 1, j, k)));
        }
        if (j != mesh.jmino()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              2, &(traversing_links(i, j - 1, k)));
        }
        if (j != mesh.jmaxo()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              3, &(traversing_links(i, j + 1, k)));
        }
        if (k != mesh.kmino()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              4, &(traversing_links(i, j, k - 1)));
        }
        if (k != mesh.kmaxo()) {
          traversing_links(i, j, k).setEdgeConnectivity(
              5, &(traversing_links(i, j, k + 1)));
        }
      }
    }
  }
  return traversing_links;
}

Data<double> initializeVolumeFraction(
    const Data<IRL::PlanarSeparator>& a_interface) {
  const BasicMesh& mesh = a_interface.getMesh();
  Data<double> liquid_volume_fraction(mesh);
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        const IRL::Pt lower_cell_pt(mesh.x(i), mesh.y(j), mesh.z(k));
        const IRL::Pt upper_cell_pt(mesh.x(i + 1), mesh.y(j + 1),
                                    mesh.z(k + 1));
        const auto cell = IRL::RectangularCuboid::fromBoundingPts(
            lower_cell_pt, upper_cell_pt);
        liquid_volume_fraction(i, j, k) =
            IRL::getVolumeFraction(cell, a_interface(i, j, k));
      }
    }
  }
  return liquid_volume_fraction;
}

void advanceState(const double a_dt, Data<double>* a_liquid_volume_fraction,
                  IRLReconstructionsPack* a_reconstructions) {
  updateVOF(a_dt, a_reconstructions->traversing_links,
            a_liquid_volume_fraction);
  updateReconstruction(*a_liquid_volume_fraction,
                       &(*a_reconstructions).liquid_gas_interface);
}

void updateVOF(const double a_dt,
               const Data<IRL::LocalizedSeparatorLink>& a_traversing_link,
               Data<double>* a_liquid_volume_fraction) {
  const BasicMesh& mesh = a_liquid_volume_fraction->getMesh();
  // For now, naively advect everywhere in domain
  const int k = mesh.kmin();
  for (int i = mesh.imin(); i <= mesh.imax(); ++i) {
    for (int j = mesh.jmin(); j <= mesh.jmax(); ++j) {
      auto cell = IRL::RectangularCuboid::fromBoundingPts(
          IRL::Pt(mesh.x(i), mesh.y(j), mesh.z(k)),
          IRL::Pt(mesh.x(i + 1), mesh.y(j + 1), mesh.z(k + 1)));
      // Get the back project CappedDodecahedron.
      IRL::Dodecahedron transported_cell;
      for (IRL::UnsignedIndex_t n = 0; n < 8; ++n) {
        transported_cell[n] = backProjectVertex(-a_dt, cell[n]);
      }
      (*a_liquid_volume_fraction)(i, j, k) = IRL::getVolumeMoments<IRL::Volume>(
          transported_cell, a_traversing_link(i, j, k));
      (*a_liquid_volume_fraction)(i, j, k) /=
          transported_cell.calculateVolume();
    }
  }
  a_liquid_volume_fraction->updateBorder();
}

// First order Euler for known velocity field.
IRL::Pt backProjectVertex(const double a_dt, const IRL::Pt& a_pt) {
  return IRL::Pt(-2.0 * M_PI * a_pt[1] * a_dt, 2.0 * M_PI * a_pt[0] * a_dt,
                 a_pt[2]) +
         a_pt;
}

void updateReconstruction(const Data<double>& a_liquid_volume_fraction,
                          Data<IRL::PlanarSeparator>* a_liquid_gas_interface) {
  IRL::ELVIRANeighborhood neighborhood;
  const BasicMesh& mesh = a_liquid_volume_fraction.getMesh();
  neighborhood.resize(9);
  IRL::RectangularCuboid cells[9];
  // Loop over cells in domain. Skip if cell is not mixed phase.
  const int k = 0;
  const int kk = 0;
  for (int i = mesh.imin(); i <= mesh.imax(); ++i) {
    for (int j = mesh.jmin(); j <= mesh.jmax(); ++j) {
      if (a_liquid_volume_fraction(i, j, k) < IRL::global_constants::VF_LOW ||
          a_liquid_volume_fraction(i, j, k) > IRL::global_constants::VF_HIGH) {
        const double distance =
            std::copysign(1.0, a_liquid_volume_fraction(i, j, k) - 0.5);
        (*a_liquid_gas_interface)(i, j, k) = IRL::PlanarSeparator::fromOnePlane(
            IRL::Plane(IRL::Normal(0.0, 0.0, 0.0), distance));
        continue;
      }
      // Build surrounding stencil information for ELVIRA.
      for (int ii = i - 1; ii < i + 2; ++ii) {
        for (int jj = j - 1; jj < j + 2; ++jj) {
          // Reversed order, bad for cache locality but thats okay..
          cells[(jj - j + 1) * 3 + (ii - i + 1)] =
              IRL::RectangularCuboid::fromBoundingPts(
                  IRL::Pt(mesh.x(ii), mesh.y(jj), mesh.z(kk)),
                  IRL::Pt(mesh.x(ii + 1), mesh.y(jj + 1), mesh.z(kk + 1)));
          neighborhood.setMember(&cells[(jj - j + 1) * 3 + (ii - i + 1)],
                                 &a_liquid_volume_fraction(ii, jj, 0), ii - i,
                                 jj - j);
        }
      }
      // Now perform actual ELVIRA and obtain interface PlanarSeparator
      (*a_liquid_gas_interface)(i, j, k) =
          reconstructionWithELVIRA2D(neighborhood);
    }
  }
  // Update border with simple ghost-cell fill and correct distances for
  // assumed periodic boundary
  a_liquid_gas_interface->updateBorder();
  correctInterfacePlaneBorders(a_liquid_gas_interface);
}

void correctInterfacePlaneBorders(
    Data<IRL::PlanarSeparator>* a_liquid_gas_interface) {
  const BasicMesh& mesh = (*a_liquid_gas_interface).getMesh();
  // Fix distances in reconstruction for periodic boundary

  // x- boundary
  for (int i = mesh.imino(); i < mesh.imin(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() - plane.normal()[0] * mesh.lx();
        }
      }
    }
  }

  // x+ boundary
  for (int i = mesh.imax() + 1; i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() + plane.normal()[0] * mesh.lx();
        }
      }
    }
  }

  // y- boundary
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j < mesh.jmin(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() - plane.normal()[1] * mesh.ly();
        }
      }
    }
  }

  // y+ boundary
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmax() + 1; j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k <= mesh.kmaxo(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() + plane.normal()[1] * mesh.ly();
        }
      }
    }
  }

  // z- boundary
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmino(); k < mesh.kmin(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() - plane.normal()[2] * mesh.lz();
        }
      }
    }
  }

  // z+ boundary
  for (int i = mesh.imino(); i <= mesh.imaxo(); ++i) {
    for (int j = mesh.jmino(); j <= mesh.jmaxo(); ++j) {
      for (int k = mesh.kmax() + 1; k <= mesh.kmaxo(); ++k) {
        for (auto& plane : (*a_liquid_gas_interface)(i, j, k)) {
          plane.distance() = plane.distance() - plane.normal()[2] * mesh.lz();
        }
      }
    }
  }
}
