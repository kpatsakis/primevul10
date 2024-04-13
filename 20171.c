static void util_set_pen(wmfAPI * API, wmfDC * dc)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  wmfPen
    *pen = 0;

  double
    pen_width,
    pixel_width;

  unsigned int
    pen_style;

  pen = WMF_DC_PEN(dc);

  pen_width = (WMF_PEN_WIDTH(pen) + WMF_PEN_HEIGHT(pen)) / 2;

  /* Pixel width is inverse of pixel scale */
  pixel_width = (((double) 1 / (ddata->scale_x)) +
                 ((double) 1 / (ddata->scale_y))) / 2;

  /* Don't allow pen_width to be much less than pixel_width in order
     to avoid dissapearing or spider-web lines */
  pen_width = MagickMax(pen_width, pixel_width*0.8);

  pen_style = (unsigned int) WMF_PEN_STYLE(pen);

  /* Pen style specified? */
  if (pen_style == PS_NULL)
    {
      draw_stroke_color_string(WmfDrawingWand,"none");
      return;
    }

  DrawSetStrokeAntialias(WmfDrawingWand, MagickTrue );
  DrawSetStrokeWidth(WmfDrawingWand, (unsigned long) MagickMax(0.0, pen_width));

  {
    LineCap
      linecap;

    switch ((unsigned int) WMF_PEN_ENDCAP(pen))
      {
      case PS_ENDCAP_SQUARE:
        linecap = SquareCap;
        break;
      case PS_ENDCAP_ROUND:
        linecap = RoundCap;
        break;
      case PS_ENDCAP_FLAT:
      default:
        linecap = ButtCap;
        break;
      }
    DrawSetStrokeLineCap(WmfDrawingWand, linecap);
  }

  {
    LineJoin
      linejoin;

    switch ((unsigned int) WMF_PEN_JOIN(pen))
      {
      case PS_JOIN_BEVEL:
        linejoin = BevelJoin;
        break;
      case PS_JOIN_ROUND:
        linejoin = RoundJoin;
        break;
      case PS_JOIN_MITER:
      default:
        linejoin = MiterJoin;
        break;
      }
    DrawSetStrokeLineJoin(WmfDrawingWand,linejoin);
  }

  {
    double
      dasharray[7];

    switch (pen_style)
      {
      case PS_DASH:    /* -------  */
        {
          /* Pattern 18,7 */
          dasharray[0] = pixel_width * 18;
          dasharray[1] = pixel_width * 7;
          dasharray[2] = 0;

          DrawSetStrokeAntialias(WmfDrawingWand,MagickFalse);
          (void) DrawSetStrokeDashArray(WmfDrawingWand,2,dasharray);
          break;
        }
      case PS_ALTERNATE:
      case PS_DOT:    /* .......  */
        {
          /* Pattern 3,3 */
          dasharray[0] = pixel_width * 3;
          dasharray[1] = pixel_width * 3;
          dasharray[2] = 0;

          DrawSetStrokeAntialias(WmfDrawingWand,MagickFalse);
          (void) DrawSetStrokeDashArray(WmfDrawingWand,2,dasharray);
          break;
        }
      case PS_DASHDOT:    /* _._._._  */
        {
          /* Pattern 9,6,3,6 */
          dasharray[0] = pixel_width * 9;
          dasharray[1] = pixel_width * 6;
          dasharray[2] = pixel_width * 3;
          dasharray[3] = pixel_width * 6;
          dasharray[4] = 0;

          DrawSetStrokeAntialias(WmfDrawingWand,MagickFalse);
          (void) DrawSetStrokeDashArray(WmfDrawingWand,4,dasharray);
          break;
        }
      case PS_DASHDOTDOT:  /* _.._.._  */
        {
          /* Pattern 9,3,3,3,3,3 */
          dasharray[0] = pixel_width * 9;
          dasharray[1] = pixel_width * 3;
          dasharray[2] = pixel_width * 3;
          dasharray[3] = pixel_width * 3;
          dasharray[4] = pixel_width * 3;
          dasharray[5] = pixel_width * 3;
          dasharray[6] = 0;

          DrawSetStrokeAntialias(WmfDrawingWand,MagickFalse);
          (void) DrawSetStrokeDashArray(WmfDrawingWand,6,dasharray);
          break;
        }
      case PS_INSIDEFRAME:  /* There is nothing to do in this case... */
      case PS_SOLID:
      default:
        {
          (void) DrawSetStrokeDashArray(WmfDrawingWand,0,(double *) NULL);
          break;
        }
      }
  }

  draw_stroke_color_rgb(API,WMF_PEN_COLOR(pen));
}