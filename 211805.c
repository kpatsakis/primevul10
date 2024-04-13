static int get_php_tiff_bytes_per_format(int format) {
  int size = sizeof(php_tiff_bytes_per_format)/sizeof(int);
  if (format >= size) {
    raise_warning("Invalid format %d", format);
    format = 0;
  }
  return php_tiff_bytes_per_format[format];
}