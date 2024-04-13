static void ipa_device_begin(wmfAPI * API)
{
  char
    comment[MagickPathExtent],
    *url;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Make SVG output happy */
  (void) PushDrawingWand(WmfDrawingWand);

  DrawSetViewbox(WmfDrawingWand,0,0,ddata->image->columns,ddata->image->rows);

  url=GetMagickHomeURL();
  (void) FormatLocaleString(comment,MagickPathExtent,
    "Created by ImageMagick %s",url);
  url=DestroyString(url);
  DrawComment(WmfDrawingWand,comment);

  /* Scale width and height to image */
  DrawScale(WmfDrawingWand, ddata->scale_x, ddata->scale_y);

  /* Translate to TL corner of bounding box */
  DrawTranslate(WmfDrawingWand, ddata->translate_x, ddata->translate_y);

  /* Apply rotation */
  DrawRotate(WmfDrawingWand, ddata->rotate);

  if (ddata->image_info->texture == NULL)
    {
      PixelWand
        *background_color;

      /* Draw rectangle in background color */
      background_color=NewPixelWand();
      PixelSetPixelColor(background_color,&ddata->image->background_color);
      DrawSetFillColor(WmfDrawingWand,background_color);
      background_color=DestroyPixelWand(background_color);
      DrawRectangle(WmfDrawingWand,
                     XC(ddata->bbox.TL.x),YC(ddata->bbox.TL.y),
                     XC(ddata->bbox.BR.x),YC(ddata->bbox.BR.y));
    }
  else
    {
      /* Draw rectangle with texture image the SVG way */
      Image
        *image;

      ImageInfo
        *image_info;

      ExceptionInfo
        *exception;

      exception=AcquireExceptionInfo();

      image_info = CloneImageInfo((ImageInfo *) 0);
      (void) CopyMagickString(image_info->filename,ddata->image_info->texture,
        MagickPathExtent);
      if ( ddata->image_info->size )
        CloneString(&image_info->size,ddata->image_info->size);

      image = ReadImage(image_info,exception);
      (void) DestroyExceptionInfo(exception);
      image_info=DestroyImageInfo(image_info);
      if (image)
        {
          char
            pattern_id[MagickPathExtent];

          MagickWand
            *magick_wand;

          (void) CopyMagickString(image->magick,"MIFF",MagickPathExtent);
          DrawPushDefs(WmfDrawingWand);
          draw_pattern_push(API,ddata->pattern_id,image->columns,image->rows);
          magick_wand=NewMagickWandFromImage(image);
          (void) DrawComposite(WmfDrawingWand,CopyCompositeOp,0,0,
            image->columns,image->rows,magick_wand);
          magick_wand=DestroyMagickWand(magick_wand);
          (void) DrawPopPattern(WmfDrawingWand);
          DrawPopDefs(WmfDrawingWand);
          (void) FormatLocaleString(pattern_id,MagickPathExtent,"#brush_%lu",
            ddata->pattern_id);
          (void) DrawSetFillPatternURL(WmfDrawingWand,pattern_id);
          ++ddata->pattern_id;
          DrawRectangle(WmfDrawingWand,
            XC(ddata->bbox.TL.x),YC(ddata->bbox.TL.y),
            XC(ddata->bbox.BR.x),YC(ddata->bbox.BR.y));
          image=DestroyImageList(image);
        }
      else
        {
          LogMagickEvent(CoderEvent,GetMagickModule(),
            "reading texture image failed!");
        }
    }

  DrawSetClipRule(WmfDrawingWand,EvenOddRule); /* Default for WMF is ALTERNATE polygon fill mode */
  draw_fill_color_string(WmfDrawingWand,"none"); /* Default brush is WHITE_BRUSH */
  draw_stroke_color_string(WmfDrawingWand,"none"); /* Default pen is BLACK_PEN */
  DrawSetStrokeLineCap(WmfDrawingWand,ButtCap); /* Default linecap is PS_ENDCAP_FLAT */
  DrawSetStrokeLineJoin(WmfDrawingWand,MiterJoin); /* Default linejoin is PS_JOIN_MITER */
  draw_under_color_string(WmfDrawingWand,"white"); /* Default text box is white */
}