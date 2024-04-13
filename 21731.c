gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass       *operation_class;
  GObjectClass             *object_class;

  operation_class = GEGL_OPERATION_CLASS (klass);
  object_class    = G_OBJECT_CLASS (klass);

  object_class->finalize = finalize;

  operation_class->process = process;
  operation_class->get_bounding_box = get_bounding_box;
  operation_class->get_cached_region = get_cached_region;;

  gegl_operation_class_set_keys (operation_class,
        "name"       , "gegl:magick-load",
        "categories" , "hidden",
        "description",
        _("Image Magick wrapper using the png op."),
        NULL);
}