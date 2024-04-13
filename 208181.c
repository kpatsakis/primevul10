test_random_numbers (xd3_stream *stream, int ignore)
{
  usize_t i;
  usize_t sum = 0;
  usize_t mean = 50;
  usize_t n_rounds = 1000000;
  double average, error;
  double allowed_error = 0.1;

  mt_init (& static_mtrand, 0x9f73f7fe);

  for (i = 0; i < n_rounds; i += 1)
    {
      sum += mt_exp_rand (mean, USIZE_T_MAX);
    }

  average = (double) sum / (double) n_rounds;
  error   = average - (double) mean;

  if (error < allowed_error && error > -allowed_error)
    {
      return 0;
    }

  /*XPR(NT "error is %f\n", error);*/
  stream->msg = "random distribution looks broken";
  return XD3_INTERNAL;
}