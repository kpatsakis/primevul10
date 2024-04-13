static void draw_fill_color_string(DrawingWand *drawing_wand,const char *color)
{
  PixelWand
    *fill_color;

  fill_color=NewPixelWand();
  PixelSetColor(fill_color,color);
  DrawSetFillColor(drawing_wand,fill_color);
  fill_color=DestroyPixelWand(fill_color);
}