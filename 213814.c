ecma_op_container_foreach (ecma_value_t this_arg, /**< this argument */
                           ecma_value_t predicate, /**< callback function */
                           ecma_value_t predicate_this_arg, /**< this argument for
                                                             *   invoke predicate */
                           lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  if (!ecma_op_is_callable (predicate))
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Callback function is not callable."));
  }

  JERRY_ASSERT (ecma_is_value_object (predicate));
  ecma_object_t *func_object_p = ecma_get_object_from_value (predicate);
  ecma_value_t ret_value = ECMA_VALUE_UNDEFINED;

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  uint8_t entry_size = ecma_op_container_entry_size (lit_id);

  for (uint32_t i = 0; i < ECMA_CONTAINER_ENTRY_COUNT (container_p); i += entry_size)
  {
    ecma_value_t *entry_p = ECMA_CONTAINER_START (container_p) + i;

    if (ecma_is_value_empty (*entry_p))
    {
      continue;
    }

    ecma_value_t key_arg = *entry_p;
    ecma_value_t value_arg = ecma_op_container_get_value (entry_p, lit_id);

    ecma_value_t call_args[] = { value_arg, key_arg, this_arg };
    ecma_value_t call_value = ecma_op_function_call (func_object_p, predicate_this_arg, call_args, 3);

    if (ECMA_IS_VALUE_ERROR (call_value))
    {
      ret_value = call_value;
      break;
    }

    ecma_free_value (call_value);
  }

  return ret_value;
} /* ecma_op_container_foreach */