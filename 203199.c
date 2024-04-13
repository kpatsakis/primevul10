print_freq_ppm(double f)
{
  if (fabs(f) < 99999.5) {
    printf("%10.3f", f);
  } else {
    printf("%10.0f", f);
  }
}