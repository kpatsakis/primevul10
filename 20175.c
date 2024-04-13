static void ipa_region_frame(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{
  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  if (TO_FILL(poly_rect) || TO_DRAW(poly_rect))
    {
      long
        i;

      draw_fill_color_string(WmfDrawingWand,"none");
      util_set_brush(API, poly_rect->dc, BrushApplyStroke);

      for (i = 0; i < (long) poly_rect->count; i++)
        {
          DrawRectangle(WmfDrawingWand,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
    }

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}