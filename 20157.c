static void util_draw_arc(wmfAPI * API,
          wmfDrawArc_t * draw_arc, magick_arc_t finish)
{
  wmfD_Coord
    BR,
    O,
    TL,
    center,
    end,
    start;

  double
    phi_e = 360,
    phi_s = 0;

  double
    Rx,
    Ry;

  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  if (TO_FILL(draw_arc) || TO_DRAW(draw_arc))
    {
      center.x = (draw_arc->TL.x + draw_arc->BR.x) / 2;
      center.y = (draw_arc->TL.y + draw_arc->BR.y) / 2;
      start = center;
      end = center;

      if (finish != magick_arc_ellipse)
        {
          draw_arc->start.x += center.x;
          draw_arc->start.y += center.y;

          draw_arc->end.x += center.x;
          draw_arc->end.y += center.y;
        }

      TL = draw_arc->TL;
      BR = draw_arc->BR;

      O = center;

      if (finish != magick_arc_ellipse)
        {
          start = draw_arc->start;
          end = draw_arc->end;
        }

      Rx = (BR.x - TL.x) / 2;
      Ry = (BR.y - TL.y) / 2;

      if (finish != magick_arc_ellipse)
        {
          start.x -= O.x;
          start.y -= O.y;

          end.x -= O.x;
          end.y -= O.y;

          phi_s = atan2((double) start.y, (double) start.x) * 180 / MagickPI;
          phi_e = atan2((double) end.y, (double) end.x) * 180 / MagickPI;

          if (phi_e <= phi_s)
            phi_e += 360;
        }

      util_set_pen(API, draw_arc->dc);
      if (finish == magick_arc_open)
        draw_fill_color_string(WmfDrawingWand,"none");
      else
        util_set_brush(API, draw_arc->dc, BrushApplyFill);

      if (finish == magick_arc_ellipse)
        DrawEllipse(WmfDrawingWand, XC(O.x), YC(O.y), Rx, Ry, 0, 360);
      else if (finish == magick_arc_pie)
        {
          DrawPathStart(WmfDrawingWand);
          DrawPathMoveToAbsolute(WmfDrawingWand, XC(O.x+start.x),
            YC(O.y+start.y));
          DrawPathEllipticArcAbsolute(WmfDrawingWand, Rx, Ry, 0, MagickFalse,
            MagickTrue, XC(O.x+end.x), YC(O.y+end.y));
          DrawPathLineToAbsolute(WmfDrawingWand, XC(O.x), YC(O.y));
          DrawPathClose(WmfDrawingWand);
          DrawPathFinish(WmfDrawingWand);
        }
        else if (finish == magick_arc_chord)
        {
          DrawArc(WmfDrawingWand, XC(draw_arc->TL.x), YC(draw_arc->TL.y),
            XC(draw_arc->BR.x), XC(draw_arc->BR.y), phi_s, phi_e);
          DrawLine(WmfDrawingWand, XC(draw_arc->BR.x-start.x),
            YC(draw_arc->BR.y-start.y), XC(draw_arc->BR.x-end.x),
            YC(draw_arc->BR.y-end.y));
        }
        else      /* if (finish == magick_arc_open) */
          DrawArc(WmfDrawingWand, XC(draw_arc->TL.x), YC(draw_arc->TL.y),
            XC(draw_arc->BR.x), XC(draw_arc->BR.y), phi_s, phi_e);
    }

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);
}