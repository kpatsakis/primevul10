ecma_op_container_get (ecma_value_t this_arg, /**< this argument */
                       ecma_value_t key_arg, /**< key argument */
                       lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

#if ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP)
  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL && !ecma_is_value_object (key_arg))
  {
    return ECMA_VALUE_UNDEFINED;
  }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) */

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  if (ECMA_CONTAINER_GET_SIZE (container_p) == 0)
  {
    return ECMA_VALUE_UNDEFINED;
  }

  ecma_value_t *entry_p = ecma_op_internal_buffer_find (container_p, key_arg, lit_id);

  if (entry_p == NULL)
  {
    return ECMA_VALUE_UNDEFINED;
  }

  return ecma_copy_value (((ecma_container_pair_t *) entry_p)->value);
} /* ecma_op_container_get */