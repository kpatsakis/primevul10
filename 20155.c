static void ipa_device_end(wmfAPI * API)
{
  wmf_magick_t
    *ddata = WMF_MAGICK_GetData(API);

  /* Reset any existing clip paths by popping wand */
  if (ddata->clipping)
    (void) PopDrawingWand(WmfDrawingWand);
  ddata->clipping = MagickFalse;

  /* Make SVG output happy */
  (void) PopDrawingWand(WmfDrawingWand);
}