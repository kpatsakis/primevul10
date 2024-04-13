static void ipa_draw_text(wmfAPI * API, wmfDrawText_t * draw_text)
{
  double
    angle = 0,      /* text rotation angle */
    bbox_height,    /* bounding box height */
    bbox_width,      /* bounding box width */
    pointsize = 0;    /* pointsize to output font with desired height */

  ExceptionInfo
    *exception;

  TypeMetric
    metrics;

  wmfD_Coord
    BL,        /* bottom left of bounding box */
    BR,        /* bottom right of bounding box */
    TL,        /* top left of bounding box */
    TR;        /* top right of bounding box */

  wmfD_Coord
    point;      /* text placement point */

  wmfFont
    *font;

  wmf_magick_t
    * ddata = WMF_MAGICK_GetData(API);

  point = draw_text->pt;

  /* Choose bounding box and calculate its width and height */
  {
    double dx,
      dy;

    if ( draw_text->flags)
      {
        TL = draw_text->TL;
        BR = draw_text->BR;
        TR.x = draw_text->BR.x;
        TR.y = draw_text->TL.y;
        BL.x = draw_text->TL.x;
        BL.y = draw_text->BR.y;
      }
    else
      {
        TL = draw_text->bbox.TL;
        BR = draw_text->bbox.BR;
        TR = draw_text->bbox.TR;
        BL = draw_text->bbox.BL;
      }
    dx = ((TR.x - TL.x) + (BR.x - BL.x)) / 2;
    dy = ((TR.y - TL.y) + (BR.y - BL.y)) / 2;
    bbox_width = hypot(dx,dy);
    dx = ((BL.x - TL.x) + (BR.x - TR.x)) / 2;
    dy = ((BL.y - TL.y) + (BR.y - TR.y)) / 2;
    bbox_height = hypot(dx,dy);
  }

  font = WMF_DC_FONT(draw_text->dc);

  /* Convert font_height to equivalent pointsize */
  exception=ddata->exception;
  pointsize = util_pointsize( API, font, draw_text->str, draw_text->font_height, exception);

  /* Save graphic wand */
  (void) PushDrawingWand(WmfDrawingWand);

  (void) bbox_width;
  (void) bbox_height;
#if 0
  printf("\nipa_draw_text\n");
  printf("Text                    = \"%s\"\n", draw_text->str);
  /* printf("WMF_FONT_NAME:          = \"%s\"\n", WMF_FONT_NAME(font)); */
  printf("WMF_FONT_PSNAME:        = \"%s\"\n", WMF_FONT_PSNAME(font));
  printf("Bounding box            TL=%g,%g BR=%g,%g\n",
         TL.x, TL.y, BR.x, BR.y );
  /* printf("Text box                = %gx%g\n", bbox_width, bbox_height); */
  /* printf("WMF_FONT_HEIGHT         = %i\n", (int)WMF_FONT_HEIGHT(font)); */
  printf("Pointsize               = %g\n", pointsize);
  fflush(stdout);
#endif

  /*
   * Obtain font metrics if required
   *
   */
  if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER) ||
      (WMF_TEXT_UNDERLINE(font)) || (WMF_TEXT_STRIKEOUT(font)))
    {
      Image
        *image = ddata->image;

      DrawInfo
        *draw_info;

      draw_info=ddata->draw_info;
      draw_info->font=WMF_FONT_PSNAME(font);
      draw_info->pointsize = pointsize;
      draw_info->text=draw_text->str;

      if (GetTypeMetrics(image, draw_info, &metrics, exception) != MagickFalse)
        {
          /* Center the text if it is not yet centered and should be */
          if ((WMF_DC_TEXTALIGN(draw_text->dc) & TA_CENTER))
            {
              double
                text_width = metrics.width * (ddata->scale_y / ddata->scale_x);

#if defined(MAGICKCORE_WMF_DELEGATE)
              point.x -= text_width / 2;
#else
              point.x += bbox_width / 2 - text_width / 2;
#endif
            }
        }
      draw_info->font=NULL;
      draw_info->text=NULL;
    }

  /* Set text background color */
  if (draw_text->flags & ETO_OPAQUE)
    {
      /* Draw bounding-box background color (META_EXTTEXTOUT mode) */
      draw_stroke_color_string(WmfDrawingWand,"none");
      draw_fill_color_rgb(API,WMF_DC_BACKGROUND(draw_text->dc));
      DrawRectangle(WmfDrawingWand,
                    XC(draw_text->TL.x),YC(draw_text->TL.y),
                    XC(draw_text->BR.x),YC(draw_text->BR.y));
      draw_fill_color_string(WmfDrawingWand,"none");
    }
  else
    {
      /* Set text undercolor */
      if (WMF_DC_OPAQUE(draw_text->dc))
        {
          wmfRGB
            *box = WMF_DC_BACKGROUND(draw_text->dc);

          PixelWand
            *under_color;

          under_color=NewPixelWand();
          PixelSetRedQuantum(under_color,ScaleCharToQuantum(box->r));
          PixelSetGreenQuantum(under_color,ScaleCharToQuantum(box->g));
          PixelSetBlueQuantum(under_color,ScaleCharToQuantum(box->b));
          PixelSetAlphaQuantum(under_color,OpaqueAlpha);
          DrawSetTextUnderColor(WmfDrawingWand,under_color);
          under_color=DestroyPixelWand(under_color);
        }
      else
        draw_under_color_string(WmfDrawingWand,"none");
    }

  /* Set text clipping (META_EXTTEXTOUT mode) */
  if ( draw_text->flags & ETO_CLIPPED)
    {
    }

  /* Set stroke color */
  draw_stroke_color_string(WmfDrawingWand,"none");

  /* Set fill color */
  draw_fill_color_rgb(API,WMF_DC_TEXTCOLOR(draw_text->dc));

  /* Output font size */
  (void) DrawSetFontSize(WmfDrawingWand,pointsize);

  /* Output Postscript font name */
  (void) DrawSetFont(WmfDrawingWand, WMF_FONT_PSNAME(font));

  /* Translate coordinates so target is 0,0 */
  DrawTranslate(WmfDrawingWand, XC(point.x), YC(point.y));

  /* Transform horizontal scale to draw text at 1:1 ratio */
  DrawScale(WmfDrawingWand, ddata->scale_y / ddata->scale_x, 1.0);

  /* Apply rotation */
  /* ImageMagick's drawing rotation is clockwise from horizontal
     while WMF drawing rotation is counterclockwise from horizontal */
  angle = fabs(RadiansToDegrees(2 * MagickPI - WMF_TEXT_ANGLE(font)));
  if (angle == 360)
    angle = 0;
  if (angle != 0)
    DrawRotate(WmfDrawingWand, angle);

  /*
   * Render text
   *
   */

  /* Output string */
  DrawAnnotation(WmfDrawingWand, 0, 0, (unsigned char*)draw_text->str);

  /* Underline text the Windows way (at the bottom) */
  if (WMF_TEXT_UNDERLINE(font))
    {
      double
        line_height;

      wmfD_Coord
        ulBR,      /* bottom right of underline rectangle */
        ulTL;      /* top left of underline rectangle */

      line_height = ((double)1/(ddata->scale_x))*metrics.underline_thickness;
      if (metrics.underline_thickness < 1.5)
        line_height *= 0.55;
      ulTL.x = 0;
      ulTL.y = fabs(metrics.descent) - line_height;
      ulBR.x = metrics.width;
      ulBR.y = fabs(metrics.descent);

      DrawRectangle(WmfDrawingWand,
                    XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));
    }

  /* Strikeout text the Windows way */
  if (WMF_TEXT_STRIKEOUT(font))
    {
      double line_height;

      wmfD_Coord
        ulBR,      /* bottom right of strikeout rectangle */
        ulTL;      /* top left of strikeout rectangle */

      line_height = ((double)1/(ddata->scale_x))*metrics.underline_thickness;

      if (metrics.underline_thickness < 2.0)
        line_height *= 0.55;
      ulTL.x = 0;
      ulTL.y = -(((double) metrics.ascent) / 2 + line_height / 2);
      ulBR.x = metrics.width;
      ulBR.y = -(((double) metrics.ascent) / 2 - line_height / 2);

      DrawRectangle(WmfDrawingWand,
                    XC(ulTL.x), YC(ulTL.y), XC(ulBR.x), YC(ulBR.y));

    }

  /* Restore graphic wand */
  (void) PopDrawingWand(WmfDrawingWand);

#if 0
  (void) PushDrawingWand(WmfDrawingWand);
  draw_stroke_color_string(WmfDrawingWand,"red");
  draw_fill_color_string(WmfDrawingWand,"none");
  DrawRectangle(WmfDrawingWand,
                XC(TL.x), YC(TL.y),
                XC(BR.x), YC(BR.y));
  draw_stroke_color_string(WmfDrawingWand,"none");
  (void) PopDrawingWand(WmfDrawingWand);
#endif

}