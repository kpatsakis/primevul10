static void ipa_device_open(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  ddata->pattern_id = 0;
  ddata->clipping = MagickFalse;
  ddata->clip_mask_id = 0;

  ddata->push_depth = 0;

  ddata->draw_wand = AcquireDrawingWand(ddata->draw_info,ddata->image);
}