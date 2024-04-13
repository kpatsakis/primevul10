static int php_imagefontsize(int size, int arg) {
  gdFontPtr font = php_find_gd_font(size);
  return (arg ? font->h : font->w);
}