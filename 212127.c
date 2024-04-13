static signed short php_ifd_get16s(void *Short, int motorola_intel) {
  return (signed short)php_ifd_get16u(Short, motorola_intel);
}