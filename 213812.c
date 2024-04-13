ecma_op_container_iterator_next (ecma_value_t this_val, /**< this argument */
                                 ecma_pseudo_array_type_t iterator_type) /**< type of the iterator */
{
  if (!ecma_is_value_object (this_val))
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Argument 'this' is not an object."));
  }

  ecma_object_t *obj_p = ecma_get_object_from_value (this_val);
  ecma_extended_object_t *ext_obj_p = (ecma_extended_object_t *) obj_p;

  if (ecma_get_object_type (obj_p) != ECMA_OBJECT_TYPE_PSEUDO_ARRAY
      || ext_obj_p->u.pseudo_array.type != iterator_type)
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Argument 'this' is not an iterator."));
  }

  ecma_value_t iterated_value = ext_obj_p->u.pseudo_array.u2.iterated_value;

  if (ecma_is_value_empty (iterated_value))
  {
    return ecma_create_iter_result_object (ECMA_VALUE_UNDEFINED, ECMA_VALUE_TRUE);
  }

  ecma_extended_object_t *map_object_p = (ecma_extended_object_t *) (ecma_get_object_from_value (iterated_value));
  lit_magic_string_id_t lit_id = map_object_p->u.class_prop.class_id;

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);
  uint32_t entry_count = ECMA_CONTAINER_ENTRY_COUNT (container_p);
  uint32_t index = ecma_op_iterator_get_index (obj_p);

  if (index == entry_count)
  {
    ext_obj_p->u.pseudo_array.u2.iterated_value = ECMA_VALUE_EMPTY;

    return ecma_create_iter_result_object (ECMA_VALUE_UNDEFINED, ECMA_VALUE_TRUE);
  }

  uint8_t entry_size = ecma_op_container_entry_size (lit_id);
  uint8_t iterator_kind = ext_obj_p->u.pseudo_array.extra_info;
  ecma_value_t *start_p = ECMA_CONTAINER_START (container_p);
  ecma_value_t ret_value = ECMA_VALUE_UNDEFINED;

  for (uint32_t i = index; i < entry_count; i += entry_size)
  {
    ecma_value_t *entry_p = start_p + i;

    if (ecma_is_value_empty (*entry_p))
    {
      if (i == (entry_count - entry_size))
      {
        ret_value = ecma_create_iter_result_object (ECMA_VALUE_UNDEFINED, ECMA_VALUE_TRUE);
        break;
      }

      continue;
    }

    ecma_op_iterator_set_index (obj_p, i + entry_size);

    ecma_value_t key_arg = *entry_p;
    ecma_value_t value_arg = ecma_op_container_get_value (entry_p, lit_id);

    if (iterator_kind == ECMA_ITERATOR_KEYS)
    {
      ret_value = ecma_create_iter_result_object (key_arg, ECMA_VALUE_FALSE);
    }
    else if (iterator_kind == ECMA_ITERATOR_VALUES)
    {
      ret_value = ecma_create_iter_result_object (value_arg, ECMA_VALUE_FALSE);
    }
    else
    {
      JERRY_ASSERT (iterator_kind == ECMA_ITERATOR_KEYS_VALUES);

      ecma_value_t entry_array_value;
      entry_array_value = ecma_create_array_from_iter_element (value_arg, key_arg);

      ret_value = ecma_create_iter_result_object (entry_array_value, ECMA_VALUE_FALSE);
      ecma_free_value (entry_array_value);
    }

    break;
  }

  return ret_value;
} /* ecma_op_container_iterator_next */