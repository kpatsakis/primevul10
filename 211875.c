static unsigned php_ifd_get32u(void *Long, int motorola_intel) {
  return (unsigned)php_ifd_get32s(Long, motorola_intel) & 0xffffffff;
}