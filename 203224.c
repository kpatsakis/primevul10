print_nanoseconds(double s)
{
  s = fabs(s);

  if (s < 9999.5e-9) {
    printf("%4.0fns", s * 1e9);
  } else if (s < 9999.5e-6) {
    printf("%4.0fus", s * 1e6);
  } else if (s < 9999.5e-3) {
    printf("%4.0fms", s * 1e3);
  } else if (s < 999.5) {
    printf("%5.1fs", s);
  } else if (s < 99999.5) {
    printf("%5.0fs", s);
  } else if (s < 99999.5 * 60) {
    printf("%5.0fm", s / 60);
  } else if (s < 99999.5 * 3600) {
    printf("%5.0fh", s / 3600);
  } else if (s < 99999.5 * 3600 * 24) {
    printf("%5.0fd", s / (3600 * 24));
  } else {
    printf("%5.0fy", s / (3600 * 24 * 365));
  }
}