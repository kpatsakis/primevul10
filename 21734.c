static void finalize (GObject *object)
{
  GeglOperation *op = (void*) object;
  GeglProperties *o = GEGL_PROPERTIES (op);

  g_clear_object (&o->user_data);

  G_OBJECT_CLASS (gegl_op_parent_class)->finalize (object);
}