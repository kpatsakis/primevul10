int64_t HHVM_FUNCTION(imagefontheight, int64_t font) {
  return php_imagefontsize(font, 1);
}