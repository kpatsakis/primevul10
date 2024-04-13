ecma_op_container_create (const ecma_value_t *arguments_list_p, /**< arguments list */
                          ecma_length_t arguments_list_len, /**< number of arguments */
                          lit_magic_string_id_t lit_id, /**< internal class id */
                          ecma_builtin_id_t proto_id) /**< prototype builtin id */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);
  JERRY_ASSERT (lit_id == LIT_MAGIC_STRING_MAP_UL
                || lit_id == LIT_MAGIC_STRING_SET_UL
                || lit_id == LIT_MAGIC_STRING_WEAKMAP_UL
                || lit_id == LIT_MAGIC_STRING_WEAKSET_UL);
  JERRY_ASSERT (JERRY_CONTEXT (current_new_target) != NULL);

  ecma_object_t *proto_p = ecma_op_get_prototype_from_constructor (JERRY_CONTEXT (current_new_target), proto_id);

  if (JERRY_UNLIKELY (proto_p == NULL))
  {
    return ECMA_VALUE_ERROR;
  }

  ecma_collection_t *container_p = ecma_op_create_internal_buffer ();
  ecma_object_t *object_p  = ecma_create_object (proto_p,
                                                 sizeof (ecma_extended_object_t),
                                                 ECMA_OBJECT_TYPE_CLASS);
  ecma_deref_object (proto_p);
  ecma_extended_object_t *map_obj_p = (ecma_extended_object_t *) object_p;
  map_obj_p->u.class_prop.extra_info = ECMA_CONTAINER_FLAGS_EMPTY;
  map_obj_p->u.class_prop.class_id = (uint16_t) lit_id;

  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL || lit_id == LIT_MAGIC_STRING_WEAKSET_UL)
  {
    map_obj_p->u.class_prop.extra_info |= ECMA_CONTAINER_FLAGS_WEAK;
  }

  ECMA_SET_INTERNAL_VALUE_POINTER (map_obj_p->u.class_prop.u.value, container_p);

  ecma_value_t set_value = ecma_make_object_value (object_p);
  ecma_value_t result = set_value;

#if ENABLED (JERRY_ES2015)
  if (arguments_list_len == 0)
  {
    return result;
  }

  ecma_value_t iterable = arguments_list_p[0];

  if (ecma_is_value_undefined (iterable) || ecma_is_value_null (iterable))
  {
    return result;
  }

  lit_magic_string_id_t adder_string_id;
  if (lit_id == LIT_MAGIC_STRING_MAP_UL || lit_id == LIT_MAGIC_STRING_WEAKMAP_UL)
  {
    adder_string_id = LIT_MAGIC_STRING_SET;
  }
  else
  {
    adder_string_id = LIT_MAGIC_STRING_ADD;
  }

  result = ecma_op_object_get_by_magic_id (object_p, adder_string_id);
  if (ECMA_IS_VALUE_ERROR (result))
  {
    goto cleanup_object;
  }

  if (!ecma_op_is_callable (result))
  {
    ecma_free_value (result);
    result = ecma_raise_type_error (ECMA_ERR_MSG ("add/set function is not callable."));
    goto cleanup_object;
  }

  ecma_object_t *adder_func_p = ecma_get_object_from_value (result);

  result = ecma_op_get_iterator (iterable, ECMA_VALUE_EMPTY);

  if (ECMA_IS_VALUE_ERROR (result))
  {
    goto cleanup_adder;
  }

  const ecma_value_t iter = result;

  while (true)
  {
    result = ecma_op_iterator_step (iter);

    if (ECMA_IS_VALUE_ERROR (result))
    {
      goto cleanup_iter;
    }

    if (ecma_is_value_false (result))
    {
      break;
    }

    const ecma_value_t next = result;
    result = ecma_op_iterator_value (next);
    ecma_free_value (next);

    if (ECMA_IS_VALUE_ERROR (result))
    {
      goto cleanup_iter;
    }

    if (lit_id == LIT_MAGIC_STRING_SET_UL || lit_id == LIT_MAGIC_STRING_WEAKSET_UL)
    {
      const ecma_value_t value = result;

      ecma_value_t arguments[] = { value };
      result = ecma_op_function_call (adder_func_p, set_value, arguments, 1);

      ecma_free_value (value);
    }
    else
    {
      if (!ecma_is_value_object (result))
      {
        ecma_free_value (result);
        ecma_raise_type_error (ECMA_ERR_MSG ("Iterator value is not an object."));
        result = ecma_op_iterator_close (iter);
        JERRY_ASSERT (ECMA_IS_VALUE_ERROR (result));
        goto cleanup_iter;
      }

      ecma_object_t *next_object_p = ecma_get_object_from_value (result);

      result = ecma_op_object_get_by_uint32_index (next_object_p, 0);

      if (ECMA_IS_VALUE_ERROR (result))
      {
        ecma_deref_object (next_object_p);
        ecma_op_iterator_close (iter);
        goto cleanup_iter;
      }

      const ecma_value_t key = result;

      result = ecma_op_object_get_by_uint32_index (next_object_p, 1);

      if (ECMA_IS_VALUE_ERROR (result))
      {
        ecma_deref_object (next_object_p);
        ecma_free_value (key);
        ecma_op_iterator_close (iter);
        goto cleanup_iter;
      }

      const ecma_value_t value = result;
      ecma_value_t arguments[] = { key, value };
      result = ecma_op_function_call (adder_func_p, set_value, arguments, 2);

      ecma_free_value (key);
      ecma_free_value (value);
      ecma_deref_object (next_object_p);
    }

    if (ECMA_IS_VALUE_ERROR (result))
    {
      ecma_op_iterator_close (iter);
      goto cleanup_iter;
    }

    ecma_free_value (result);
  }

  ecma_free_value (iter);
  ecma_deref_object (adder_func_p);
  return ecma_make_object_value (object_p);

cleanup_iter:
  ecma_free_value (iter);
cleanup_adder:
  ecma_deref_object (adder_func_p);
cleanup_object:
  ecma_deref_object (object_p);
#endif /* ENABLED (JERRY_ES2015) */

  return result;
} /* ecma_op_container_create */