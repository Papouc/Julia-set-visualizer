#ifndef __COMPLEX_PLAIN_H__
#define __COMPLEX_PLAIN_H__

#include <stdint.h>

#define CONVERGENCE_LIMIT 2.0

/// @brief type holding info about desired calculation
/// @struct complex_calc
///
typedef struct
{
  double n_r;
  double n_i;

  double c_r;
  double c_i;

  uint8_t n_iters;

} complex_calc;

/// @brief gets number of iterations in complex plain needed for divergence
///
/// @param n_r real part of the number
/// @param n_i complex part of the number
/// @param n upper bound of iterations
///
/// @return number of iterations
uint8_t get_iters_at_pos(complex_calc *calc);

/// @brief raise a complex number to power of 2
///
/// @param n_r real part of the number
/// @param n_i complex part of the number
/// @param res_r pointer to var keeping real part of operation result
/// @param res_i pointer to var keeping complex part of operation result
///
/// @return
void complex_pow_2(double n_r, double n_i, double *res_r, double *res_i);

/// @brief detrmine size of a complex number
///
/// @param n_r real part of the number
/// @param n_i complex part of the number
///
/// @return size (distance from origin) of a complex number
double complex_abs(double n_r, double n_i);

#endif
