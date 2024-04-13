static int php_ifd_get16u(void *Short, int motorola_intel) {
  if (motorola_intel) {
    return (((unsigned char *)Short)[0] << 8) | ((unsigned char *)Short)[1];
  } else {
    return (((unsigned char *)Short)[1] << 8) | ((unsigned char *)Short)[0];
  }
}