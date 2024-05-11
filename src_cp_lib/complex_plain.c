#include "complex_plain.h"
#include <math.h>
#include <stdbool.h>

// inner lib function only (is static making it not accessable from other files)
static uint8_t get_point_iters(complex_calc *calc, uint8_t current_iter);

uint8_t get_iters_at_pos(complex_calc *calc)
{
  return get_point_iters(calc, 0);
}

void complex_pow_2(double n_r, double n_i, double *res_r, double *res_i)
{
  *res_r = (n_r * n_r) - (n_i * n_i);
  *res_i = 2.0 * (n_r * n_i);
}

double complex_abs(double n_r, double n_i)
{
  return sqrt((n_r * n_r) + (n_i * n_i));
}

static uint8_t get_point_iters(complex_calc *calc, uint8_t current_iter)
{
  bool diverges = complex_abs(calc->n_r, calc->n_i) >= CONVERGENCE_LIMIT;
  bool exceeded_max = current_iter >= calc->n_iters;

  if (diverges || exceeded_max)
  {
    return current_iter;
  }

  // square current number
  double raised_r = 0.0;
  double raised_i = 0.0;
  complex_pow_2(calc->n_r, calc->n_i, &raised_r, &raised_i);

  // update values in calc (apply constant)
  calc->n_r = raised_r + calc->c_r;
  calc->n_i = raised_i + calc->c_i;

  return get_point_iters(calc, current_iter + 1);
}
