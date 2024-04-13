empathy_adium_data_get_type (void)
{
  static GType type_id = 0;

  if (!type_id)
    {
      type_id = g_boxed_type_register_static ("EmpathyAdiumData",
          (GBoxedCopyFunc) empathy_adium_data_ref,
          (GBoxedFreeFunc) empathy_adium_data_unref);
    }

  return type_id;
}