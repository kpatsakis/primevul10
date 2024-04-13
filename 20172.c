static double util_pointsize( wmfAPI* API, wmfFont* font, char* str, double font_height, ExceptionInfo *exception)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  Image
    *image = ddata->image;

  TypeMetric
    metrics;

  DrawInfo
    *draw_info;

  double
    pointsize = 0;

  draw_info=ddata->draw_info;
  if (draw_info == (const DrawInfo *) NULL)
    return 0;

  draw_info->font=WMF_FONT_PSNAME(font);
  draw_info->pointsize=font_height;
  draw_info->text=str;

  if (GetTypeMetrics(image, draw_info, &metrics, exception) != MagickFalse)
    {

      if (strlen(str) == 1)
        {
          pointsize = (font_height *
                       ( font_height / (metrics.ascent + fabs(metrics.descent))));
          draw_info->pointsize = pointsize;
          if (GetTypeMetrics(image, draw_info, &metrics, exception) != MagickFalse)
            pointsize *= (font_height / ( metrics.ascent + fabs(metrics.descent)));
        }
      else
        {
          pointsize = (font_height * (font_height / (metrics.height)));
          draw_info->pointsize = pointsize;
          if (GetTypeMetrics(image, draw_info, &metrics, exception) != MagickFalse)
            pointsize *= (font_height / metrics.height);

        }
#if 0
      draw_info.pointsize = pointsize;
      if (GetTypeMetrics(image, &draw_info, &metrics, exception) != MagickFalse)
        pointsize *= (font_height / (metrics.ascent + fabs(metrics.descent)));
      pointsize *= 1.114286; /* Magic number computed through trial and error */
#endif
    }

  draw_info->font=NULL;
  draw_info->text=NULL;
#if 0
  printf("String    = %s\n", str);
  printf("Font      = %s\n", WMF_FONT_PSNAME(font));
  printf("lfHeight  = %g\n", font_height);
  printf("bounds    = %g,%g %g,%g\n", metrics.bounds.x1, metrics.bounds.y1,
         metrics.bounds.x2,metrics.bounds.y2);
  printf("ascent    = %g\n", metrics.ascent);
  printf("descent   = %g\n", metrics.descent);
  printf("height    = %g\n", metrics.height);
  printf("Pointsize = %g\n", pointsize);
#endif

  return floor(pointsize);
}