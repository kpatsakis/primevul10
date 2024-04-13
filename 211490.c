static int php_jpg_get16(void *value) {
  return (((unsigned char *)value)[0] << 8) | ((unsigned char *)value)[1];
}