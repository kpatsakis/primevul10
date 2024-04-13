static void ipa_device_close(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  if (ddata->draw_wand != (DrawingWand *) NULL)
    {
      DestroyDrawingWand(ddata->draw_wand);
      ddata->draw_wand=(DrawingWand *) NULL;
    }
  if (ddata->draw_info != (DrawInfo *) NULL)
    {
      DestroyDrawInfo(ddata->draw_info);
      ddata->draw_info=(DrawInfo *)NULL;
    }
  RelinquishMagickMemory(WMF_MAGICK_GetFontData(API)->ps_name);
}