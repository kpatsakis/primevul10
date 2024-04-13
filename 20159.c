static void ipa_region_paint(wmfAPI * API, wmfPolyRectangle_t * poly_rect)
{

  if (poly_rect->count == 0)
    return;

  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  if (TO_FILL (poly_rect))
    {
      long
        i;

      draw_stroke_color_string(WmfDrawingWand,"none");
      util_set_brush(API, poly_rect->dc, BrushApplyFill);

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