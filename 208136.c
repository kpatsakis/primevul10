mt_exp_rand (uint32_t mean, uint32_t max_value)
{
  double mean_d = mean;
  double erand  = log (1.0 / (mt_random (&static_mtrand) /
			      (double)UINT32_MAX));
  uint32_t x = (uint32_t) (mean_d * erand + 0.5);

  return min (x, max_value);
}