print_seconds(unsigned long s)
{
  unsigned long d;
  if (s <= 1024) {
    printf("%4ld", s);
  } else if (s < 36000) {
    printf("%3ldm", s / 60);
  } else if (s < 345600) {
    printf("%3ldh", s / 3600);
  } else {
    d = s / 86400;
    if (d > 999) {
      printf("%3ldy", d / 365);
    } else {
      printf("%3ldd", d);
    }
  }
}