static inline char *php_ecvt(double value, int ndigit, int *decpt, int *sign) {
  return(__cvt(value, ndigit, decpt, sign, 0, 1));
}