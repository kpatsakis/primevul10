get_bounding_box (GeglOperation *operation)
{
  GeglRectangle result = {0,0,0,0};
  GeglProperties   *o = GEGL_PROPERTIES (operation);
  gint width, height;

  load_cache (o);

  g_object_get (o->user_data, "width", &width,
                               "height", &height, NULL);
  result.width  = width;
  result.height = height;
  return result;
}