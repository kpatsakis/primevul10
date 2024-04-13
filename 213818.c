ecma_op_container_unref_weak (ecma_object_t *object_p, /**< this argument */
                              ecma_value_t ref_holder) /**< key argument */
{
  ecma_string_t *weak_refs_string_p = ecma_get_magic_string (LIT_INTERNAL_MAGIC_STRING_WEAK_REFS);

  ecma_property_t *property_p = ecma_find_named_property (object_p, weak_refs_string_p);
  JERRY_ASSERT (property_p != NULL);

  ecma_collection_t *refs_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                               ECMA_PROPERTY_VALUE_PTR (property_p)->value);
  for (uint32_t i = 0; i < refs_p->item_count; i++)
  {
    if (refs_p->buffer_p[i] == ref_holder)
    {
      refs_p->buffer_p[i] = ECMA_VALUE_EMPTY;
      break;
    }
  }
} /* ecma_op_container_unref_weak */