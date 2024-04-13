static void ipa_draw_rectangle(wmfAPI * API, wmfDrawRectangle_t * draw_rect)
{
  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  if (TO_FILL(draw_rect) || TO_DRAW(draw_rect))
    {
      util_set_pen(API, draw_rect->dc);
      util_set_brush(API, draw_rect->dc, BrushApplyFill);

      if ((draw_rect->width > 0) || (draw_rect->height > 0))
        DrawRoundRectangle(WmfDrawingWand,
                           XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                           XC(draw_rect->BR.x), YC(draw_rect->BR.y),
                           draw_rect->width / 2, draw_rect->height / 2);
      else
        DrawRectangle(WmfDrawingWand,
                      XC(draw_rect->TL.x), YC(draw_rect->TL.y),
                      XC(draw_rect->BR.x), YC(draw_rect->BR.y));
    }

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}