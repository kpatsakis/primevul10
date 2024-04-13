static void ipa_bmp_draw(wmfAPI *API, wmfBMP_Draw_t *bmp_draw)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    *exception;

  Image
    *image;

  MagickWand
    *magick_wand;

  double
    height,
    width;

  PixelInfo
    white;

  if (bmp_draw->bmp.data == 0)
    return;

  image = (Image*)bmp_draw->bmp.data;
  if (!image)
     return;

  exception=ddata->exception;
  if (bmp_draw->crop.x || bmp_draw->crop.y ||
     (bmp_draw->crop.w != bmp_draw->bmp.width) ||
     (bmp_draw->crop.h != bmp_draw->bmp.height))
    {
      /* Image needs to be cropped */
      Image
        *crop_image;

      RectangleInfo
        crop_info;

      crop_info.x = bmp_draw->crop.x;
      crop_info.y = bmp_draw->crop.y;
      crop_info.width = bmp_draw->crop.w;
      crop_info.height = bmp_draw->crop.h;

      crop_image = CropImage( image, &crop_info, exception );
      if (crop_image)
        {
          image=DestroyImageList(image);
          image = crop_image;
          bmp_draw->bmp.data = (void*)image;
        }
    }

  QueryColorCompliance( "white", AllCompliance, &white, exception );

  if ( ddata->image_info->texture ||
       !(IsPixelInfoEquivalent(&ddata->image_info->background_color,&white)) ||
       ddata->image_info->background_color.alpha != OpaqueAlpha )
  {
    /*
      Set image white background to transparent so that it may be
      overlaid over non-white backgrounds.
    */
    QueryColorCompliance( "white", AllCompliance, &white, exception );
    TransparentPaintImage( image, &white, QuantumRange, MagickFalse, exception );
  }

  width = fabs(bmp_draw->pixel_width * (double) bmp_draw->crop.w);
  height = fabs(bmp_draw->pixel_height * (double) bmp_draw->crop.h);
  magick_wand=NewMagickWandFromImage(image);
  (void) DrawComposite(WmfDrawingWand, CopyCompositeOp,
    XC(bmp_draw->pt.x) * ddata->scale_x, YC(bmp_draw->pt.y) * ddata->scale_y,
    width * ddata->scale_x, height * ddata->scale_y, magick_wand);
  magick_wand=DestroyMagickWand(magick_wand);

#if 0
  printf("bmp_draw->bmp.data   = 0x%lx\n", (long)bmp_draw->bmp.data);
  printf("registry id          = %li\n", id);
  /* printf("pixel_width          = %g\n", bmp_draw->pixel_width); */
  /* printf("pixel_height         = %g\n", bmp_draw->pixel_height); */
  printf("bmp_draw->bmp WxH    = %ix%i\n", bmp_draw->bmp.width, bmp_draw->bmp.height);
  printf("bmp_draw->crop WxH   = %ix%i\n", bmp_draw->crop.w, bmp_draw->crop.h);
  printf("bmp_draw->crop x,y   = %i,%i\n", bmp_draw->crop.x, bmp_draw->crop.y);
  printf("image size WxH       = %lux%lu\n", image->columns, image->rows);
#endif
}