static void ipa_bmp_read(wmfAPI * API, wmfBMP_Read_t * bmp_read) {
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  ExceptionInfo
    *exception;

  Image
    *image;

  ImageInfo
    *image_info;

  bmp_read->bmp.data = 0;

  image_info=CloneImageInfo(ddata->image_info);
  exception=ddata->exception;
  (void) CopyMagickString(image_info->magick,"DIB",MagickPathExtent);
  if (bmp_read->width || bmp_read->height)
    {
      char
        size[MagickPathExtent];

      (void) FormatLocaleString(size,MagickPathExtent,"%ux%u",bmp_read->width,
        bmp_read->height);
      CloneString(&image_info->size,size);
    }
#if 0
  printf("ipa_bmp_read: buffer=0x%lx length=%ld, width=%i, height=%i\n",
   (long) bmp_read->buffer, bmp_read->length,
   bmp_read->width, bmp_read->height);
#endif
  image=BlobToImage(image_info, (const void *) bmp_read->buffer,
    bmp_read->length, exception);
  image_info=DestroyImageInfo(image_info);
  if (image != (Image *) NULL)
    {
#if 0
      printf("ipa_bmp_read: rows=%ld,columns=%ld\n\n", image->rows, image->columns);
#endif

      bmp_read->bmp.data   = (void*)image;
      bmp_read->bmp.width  = (U16)image->columns;
      bmp_read->bmp.height = (U16)image->rows;
    }
}