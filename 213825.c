ecma_op_container_has (ecma_value_t this_arg, /**< this argument */
                       ecma_value_t key_arg, /**< key argument */
                       lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

#if ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) || ENABLED (JERRY_ES2015_BUILTIN_WEAKSET)
  if ((map_object_p->u.class_prop.extra_info & ECMA_CONTAINER_FLAGS_WEAK) != 0
      && !ecma_is_value_object (key_arg))
  {
    return ECMA_VALUE_FALSE;
  }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) ||  ENABLED (JERRY_ES2015_BUILTIN_WEAKSET) */

  if (ECMA_CONTAINER_GET_SIZE (container_p) == 0)
  {
    return ECMA_VALUE_FALSE;
  }

  ecma_value_t *entry_p = ecma_op_internal_buffer_find (container_p, key_arg, lit_id);

  return ecma_make_boolean_value (entry_p != NULL);
} /* ecma_op_container_has */