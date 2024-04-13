ecma_op_container_set_weak (ecma_object_t *const key_p, /**< key object */
                            ecma_extended_object_t *const container_p) /**< container */
{
  if (JERRY_UNLIKELY (ecma_op_object_is_fast_array (key_p)))
  {
    ecma_fast_array_convert_to_normal (key_p);
  }

  ecma_string_t *weak_refs_string_p = ecma_get_magic_string (LIT_INTERNAL_MAGIC_STRING_WEAK_REFS);
  ecma_property_t *property_p = ecma_find_named_property (key_p, weak_refs_string_p);
  ecma_collection_t *refs_p;

  if (property_p == NULL)
  {
    ecma_property_value_t *value_p = ecma_create_named_data_property (key_p,
                                                                      weak_refs_string_p,
                                                                      ECMA_PROPERTY_CONFIGURABLE_WRITABLE,
                                                                      &property_p);
    ECMA_CONVERT_DATA_PROPERTY_TO_INTERNAL_PROPERTY (property_p);
    refs_p = ecma_new_collection ();
    ECMA_SET_INTERNAL_VALUE_POINTER (value_p->value, refs_p);
  }
  else
  {
    refs_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t, (ECMA_PROPERTY_VALUE_PTR (property_p)->value));
  }

  const ecma_value_t container_value = ecma_make_object_value ((ecma_object_t *) container_p);
  for (uint32_t i = 0; i < refs_p->item_count; i++)
  {
    if (ecma_is_value_empty (refs_p->buffer_p[i]))
    {
      refs_p->buffer_p[i] = container_value;
      return;
    }
  }

  ecma_collection_push_back (refs_p, container_value);
} /* ecma_op_container_set_weak */