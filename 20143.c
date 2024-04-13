static void draw_under_color_string(DrawingWand *drawing_wand,const char *color)
{
  PixelWand
    *under_color;

  under_color=NewPixelWand();
  PixelSetColor(under_color,color);
  DrawSetTextUnderColor(drawing_wand,under_color);
  under_color=DestroyPixelWand(under_color);
}