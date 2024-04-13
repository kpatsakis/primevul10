main_print_overflow (int x)
{
  XPR(NT "internal print buffer overflow: %d bytes\n", x);
  return XD3_INTERNAL;
}