static void ipa_draw_line(wmfAPI * API, wmfDrawLine_t * draw_line)
{
  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  if (TO_DRAW(draw_line))
    {
      util_set_pen(API, draw_line->dc);
      DrawLine(WmfDrawingWand,
               XC(draw_line->from.x), YC(draw_line->from.y),
               XC(draw_line->to.x), YC(draw_line->to.y));
    }

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}