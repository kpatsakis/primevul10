static bool php_imagechar(const Resource& image, int size, int x, int y,
                          const String& c, int color, int mode) {
  gdImagePtr im = get_valid_image_resource(image);
  if (!im) return false;
  int ch = 0;
  gdFontPtr font;

  if (mode < 2) {
    ch = (int)((unsigned char)(c.charAt(0)));
  }

  font = php_find_gd_font(size);

  switch (mode) {
  case 0:
    gdImageChar(im, font, x, y, ch, color);
    break;
  case 1:
    php_gdimagecharup(im, font, x, y, ch, color);
    break;
  case 2:
    for (int i = 0; (i < c.length()); i++) {
      gdImageChar(im, font, x, y, (int)((unsigned char)c.charAt(i)), color);
      x += font->w;
    }
    break;
  case 3:
    for (int i = 0; (i < c.length()); i++) {
      gdImageCharUp(im, font, x, y, (int)c.charAt(i), color);
      y -= font->w;
    }
    break;
  }
  return true;
}