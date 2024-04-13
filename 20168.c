static void draw_stroke_color_string(DrawingWand *drawing_wand,const char *color)
{
  PixelWand
    *stroke_color;

  stroke_color=NewPixelWand();
  PixelSetColor(stroke_color,color);
  DrawSetStrokeColor(drawing_wand,stroke_color);
  stroke_color=DestroyPixelWand(stroke_color);
}