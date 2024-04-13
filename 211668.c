static inline char *php_fcvt(double value, int ndigit, int *decpt, int *sign) {
  return(__cvt(value, ndigit, decpt, sign, 1, 1));
}