static void ipa_flood_interior(wmfAPI * API, wmfFlood_t * flood)
{
  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  draw_fill_color_rgb(API,&(flood->color));

  DrawColor(WmfDrawingWand,XC(flood->pt.x), YC(flood->pt.y),
            FillToBorderMethod);

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}