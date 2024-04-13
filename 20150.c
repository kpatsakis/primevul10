static void ipa_draw_polypolygon(wmfAPI * API, wmfPolyPoly_t* polypolygon)
{
  if (TO_FILL(polypolygon) || TO_DRAW(polypolygon))
    {
      int
        polygon,
        point;

      wmfPolyLine_t
        polyline;

      /* Save graphic wand */
      (void) PushDrawingWand(WmfDrawingWand);

      util_set_pen(API, polypolygon->dc);
      util_set_brush(API, polypolygon->dc, BrushApplyFill);

      DrawPathStart(WmfDrawingWand);
      for (polygon = 0; polygon < polypolygon->npoly; polygon++)
        {
          polyline.dc = polypolygon->dc;
          polyline.pt = polypolygon->pt[polygon];
          polyline.count = polypolygon->count[polygon];
          if ((polyline.count > 2) && polyline.pt)
            {
              DrawPathMoveToAbsolute(WmfDrawingWand,
                                     XC(polyline.pt[0].x),
                                     YC(polyline.pt[0].y));
              for (point = 1; point < polyline.count; point++)
                {
                  DrawPathLineToAbsolute(WmfDrawingWand,
                                         XC(polyline.pt[point].x),
                                         YC(polyline.pt[point].y));
                }
              DrawPathClose(WmfDrawingWand);
            }
        }
      DrawPathFinish(WmfDrawingWand);

      /* Restore graphic wand */
      (void) PopDrawingWand(WmfDrawingWand);
    }
}