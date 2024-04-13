static void ipa_region_clip(wmfAPI *API, wmfPolyRectangle_t *poly_rect)
{
  long
    i;

  wmf_magick_t
    *ddata = WMF_MAGICK_GetData (API);

  /* Reset any existing clip paths by popping wand */
  if (ddata->clipping)
    (void) PopDrawingWand(WmfDrawingWand);
  ddata->clipping = MagickFalse;

  if (poly_rect->count > 0)
    {
      char
        clip_mask_id[MagickPathExtent];

      /* Define clip path */
      ddata->clip_mask_id++;
      DrawPushDefs(WmfDrawingWand);
      (void) FormatLocaleString(clip_mask_id,MagickPathExtent,"clip_%lu",
        ddata->clip_mask_id);
      DrawPushClipPath(WmfDrawingWand,clip_mask_id);
      (void) PushDrawingWand(WmfDrawingWand);
      for (i = 0; i < (long) poly_rect->count; i++)
        {
          DrawRectangle(WmfDrawingWand,
                         XC(poly_rect->TL[i].x), YC(poly_rect->TL[i].y),
                         XC(poly_rect->BR[i].x), YC(poly_rect->BR[i].y));
        }
      (void) PopDrawingWand(WmfDrawingWand);
      DrawPopClipPath(WmfDrawingWand);
      DrawPopDefs(WmfDrawingWand);

      /* Push wand for new clip paths */
      (void) PushDrawingWand(WmfDrawingWand);
      (void) DrawSetClipPath(WmfDrawingWand,clip_mask_id);
      ddata->clipping = MagickTrue;
    }
}