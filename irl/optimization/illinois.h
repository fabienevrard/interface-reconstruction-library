// This file is part of the Interface Reconstruction Library (IRL),
// a library for interface reconstruction and computational geometry operations.
//
// Copyright (C) 2019 Robert Chiodi <robert.chiodi@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef IRL_OPTIMIZATION_ILLINOIS_H_
#define IRL_OPTIMIZATION_ILLINOIS_H_

#include <cmath>
#include <utility>

#include "irl/helpers/helper.h"

namespace IRL {
/// \brief A templated class driver that performs the
/// Illinois method of Regula-Falsi zero-finding.
///
/// This class is a general Regula-Falsi solver
/// that takes a pointer to a templated class
/// that must implement certain methods (detailed below).
/// Upon completion, the final solution should be stored
/// in the pointed-to-class because of the use of
/// `updateGuess()`.
///
/// Requirements for Optimizing class:
/// `calculateSignedScalarError()` : A method to calculate a scalar error
/// for the class with sign of (correct-guess).
/// - `updateGuess(...)` : A method that takes in the delta change
/// and computes a new guess vector (which it is storing itself)
/// - `updateBoundaries(...)` : (Only if `solveAndTrackBounds()` is being used)
/// A method to keep track of the best current solutions on the +/- side of the
/// zero.
/// - `errorTooHigh(...)` : A method that takes error
/// and returns a boolean whether the error is low
/// enough to stop optimization and return.
template <class OptimizingClass>
class Illinois {
 public:
  /// \brief Default constructor
  Illinois(void);

  /// \brief Solution that resets optimizing class and brackets, then solves.
  ///
  /// \param[in] a_otype Pointer to class being optimized
  /// \param[in] a_bracket_0 Bracket on one side of zero.
  /// \param[in] a_bracket_1 Bracket on other side of zero from a_bracet_0.
  void solve(OptimizingClass* a_otype, double a_bracket_0, double a_bracket_1);

  /// \brief Default destructor
  ~Illinois(void) = default;

 private:
  /// \brief Given two locations that bracket a zero, find the zero.
  ///
  /// \param[in] a_bracket_0 Bracket on one side of zero.
  /// \param[in] a_bracket_1 Bracket on other side of zero from a_bracet_0.
  void solve(double a_bracket_0, double a_bracket_1);

  /// \brief Set the guess and calculate the associated error.
  double calculateError(double* a_guess);

  /// \brief Pointer to object of class `OptimizingClass`
  /// that is being optimized.
  OptimizingClass* otype_m;
};

}  // namespace IRL

#include "irl/optimization/illinois.tpp"

#endif // IRL_OPTIMIZATION_ILLINOIS_H_
