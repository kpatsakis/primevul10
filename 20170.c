static void draw_stroke_color_rgb( wmfAPI* API, const wmfRGB* rgb )
{
  PixelWand
    *stroke_color;

  stroke_color=NewPixelWand();
  PixelSetRedQuantum(stroke_color,ScaleCharToQuantum(rgb->r));
  PixelSetGreenQuantum(stroke_color,ScaleCharToQuantum(rgb->g));
  PixelSetBlueQuantum(stroke_color,ScaleCharToQuantum(rgb->b));
  PixelSetAlphaQuantum(stroke_color,OpaqueAlpha);
  DrawSetStrokeColor(WmfDrawingWand,stroke_color);
  stroke_color=DestroyPixelWand(stroke_color);
}