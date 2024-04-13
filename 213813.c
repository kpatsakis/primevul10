ecma_op_iterator_get_index (ecma_object_t *iter_obj_p)  /**< iterator object pointer */
{
  uint32_t index = ((ecma_extended_object_t *) iter_obj_p)->u.pseudo_array.u1.iterator_index;

  if (JERRY_UNLIKELY (index == ECMA_ITERATOR_INDEX_LIMIT))
  {
    ecma_string_t *prop_name_p = ecma_get_magic_string (LIT_INTERNAL_MAGIC_STRING_ITERATOR_NEXT_INDEX);
    ecma_property_t *property_p = ecma_find_named_property (iter_obj_p, prop_name_p);
    ecma_property_value_t *value_p = ECMA_PROPERTY_VALUE_PTR (property_p);

    return (uint32_t) (ecma_get_number_from_value (value_p->value));
  }

  return index;
} /* ecma_op_iterator_get_index */