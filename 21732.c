process (GeglOperation         *operation,
         GeglOperationContext  *context,
         const gchar           *output_pad,
         const GeglRectangle   *result,
         gint                   level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);

  if (!o->user_data)
    return FALSE;
  /* overriding the predefined behavior */
  g_object_ref (o->user_data);
  gegl_operation_context_take_object (context, "output", G_OBJECT (o->user_data));
  return  TRUE;
}