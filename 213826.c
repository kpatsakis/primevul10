ecma_op_iterator_set_index (ecma_object_t *iter_obj_p, /**< iterator object pointer */
                            uint32_t index) /* iterator index to set */
{
  if (JERRY_UNLIKELY (index >= ECMA_ITERATOR_INDEX_LIMIT))
  {
    /* After the ECMA_ITERATOR_INDEX_LIMIT limit is reached the [[%Iterator%NextIndex]]
       property is stored as an internal property */
    ecma_string_t *prop_name_p = ecma_get_magic_string (LIT_INTERNAL_MAGIC_STRING_ITERATOR_NEXT_INDEX);
    ecma_property_t *property_p = ecma_find_named_property (iter_obj_p, prop_name_p);
    ecma_property_value_t *value_p;

    if (property_p == NULL)
    {
      value_p = ecma_create_named_data_property (iter_obj_p, prop_name_p, ECMA_PROPERTY_FLAG_WRITABLE, &property_p);
      value_p->value = ecma_make_uint32_value (index);
    }
    else
    {
      value_p = ECMA_PROPERTY_VALUE_PTR (property_p);
      value_p->value = ecma_make_uint32_value (index);
    }
  }
  else
  {
    ((ecma_extended_object_t *) iter_obj_p)->u.pseudo_array.u1.iterator_index = (uint16_t) index;
  }
} /* ecma_op_iterator_set_index */