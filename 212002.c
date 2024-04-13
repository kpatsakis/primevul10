Variant HHVM_FUNCTION(imageloadfont, const String& /*file*/) {
  throw_not_supported(__func__, "NYI");
  // If you decide to implement this, be careful to avoid the crash from
  // https://github.com/php/php-src/commit/9a60aed6d1925c98b1b40c19b40f5b4b65baa
}