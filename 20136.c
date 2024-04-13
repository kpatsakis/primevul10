static void ipa_functions(wmfAPI *API)
{
  wmf_magick_t
    *ddata = 0;

  wmfFunctionReference
    *FR = (wmfFunctionReference *) API->function_reference;

  /*
     IPA function reference links
   */
  FR->device_open = ipa_device_open;
  FR->device_close = ipa_device_close;
  FR->device_begin = ipa_device_begin;
  FR->device_end = ipa_device_end;
  FR->flood_interior = ipa_flood_interior;
  FR->flood_exterior = ipa_flood_exterior;
  FR->draw_pixel = ipa_draw_pixel;
  FR->draw_pie = ipa_draw_pie;
  FR->draw_chord = ipa_draw_chord;
  FR->draw_arc = ipa_draw_arc;
  FR->draw_ellipse = ipa_draw_ellipse;
  FR->draw_line = ipa_draw_line;
  FR->poly_line = ipa_poly_line;
  FR->draw_polygon = ipa_draw_polygon;
#if defined(MAGICKCORE_WMF_DELEGATE)
  FR->draw_polypolygon = ipa_draw_polypolygon;
#endif
  FR->draw_rectangle = ipa_draw_rectangle;
  FR->rop_draw = ipa_rop_draw;
  FR->bmp_draw = ipa_bmp_draw;
  FR->bmp_read = ipa_bmp_read;
  FR->bmp_free = ipa_bmp_free;
  FR->draw_text = ipa_draw_text;
  FR->udata_init = ipa_udata_init;
  FR->udata_copy = ipa_udata_copy;
  FR->udata_set = ipa_udata_set;
  FR->udata_free = ipa_udata_free;
  FR->region_frame = ipa_region_frame;
  FR->region_paint = ipa_region_paint;
  FR->region_clip = ipa_region_clip;

  /*
     Allocate device data structure
   */
  ddata = (wmf_magick_t *) wmf_malloc(API, sizeof(wmf_magick_t));
  if (ERR(API))
    return;

  (void) ResetMagickMemory((void *) ddata, 0, sizeof(wmf_magick_t));
  API->device_data = (void *) ddata;

  /*
     Device data defaults
   */
  ddata->image = 0;
}