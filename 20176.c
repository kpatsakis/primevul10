static void ipa_draw_pixel(wmfAPI * API, wmfDrawPixel_t * draw_pixel)
{
  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  draw_stroke_color_string(WmfDrawingWand,"none");

  draw_fill_color_rgb(API,&(draw_pixel->color));

  DrawRectangle(WmfDrawingWand,
                 XC(draw_pixel->pt.x),
                 YC(draw_pixel->pt.y),
                 XC(draw_pixel->pt.x + draw_pixel->pixel_width),
                 YC(draw_pixel->pt.y + draw_pixel->pixel_height));

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}