print_signed_nanoseconds(double s)
{
  double x;

  x = fabs(s);

  if (x < 9999.5e-9) {
    printf("%+5.0fns", s * 1e9);
  } else if (x < 9999.5e-6) {
    printf("%+5.0fus", s * 1e6);
  } else if (x < 9999.5e-3) {
    printf("%+5.0fms", s * 1e3);
  } else if (x < 999.5) {
    printf("%+6.1fs", s);
  } else if (x < 99999.5) {
    printf("%+6.0fs", s);
  } else if (x < 99999.5 * 60) {
    printf("%+6.0fm", s / 60);
  } else if (x < 99999.5 * 3600) {
    printf("%+6.0fh", s / 3600);
  } else if (x < 99999.5 * 3600 * 24) {
    printf("%+6.0fd", s / (3600 * 24));
  } else {
    printf("%+6.0fy", s / (3600 * 24 * 365));
  }
}