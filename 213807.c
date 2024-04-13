ecma_op_container_set (ecma_value_t this_arg, /**< this argument */
                       ecma_value_t key_arg, /**< key argument */
                       ecma_value_t value_arg, /**< value argument */
                       lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

#if ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) ||  ENABLED (JERRY_ES2015_BUILTIN_WEAKSET)
  if ((map_object_p->u.class_prop.extra_info & ECMA_CONTAINER_FLAGS_WEAK) != 0
      && !ecma_is_value_object (key_arg))
  {
    return ecma_raise_type_error (ECMA_ERR_MSG ("Key must be an object"));
  }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) ||  ENABLED (JERRY_ES2015_BUILTIN_WEAKSET) */

  ecma_value_t *entry_p = ecma_op_internal_buffer_find (container_p, key_arg, lit_id);

  if (entry_p == NULL)
  {
    ecma_op_internal_buffer_append (container_p,
                                    ecma_op_container_set_noramlize_zero (key_arg),
                                    value_arg,
                                    lit_id);

#if ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) ||  ENABLED (JERRY_ES2015_BUILTIN_WEAKSET)
    if ((map_object_p->u.class_prop.extra_info & ECMA_CONTAINER_FLAGS_WEAK) != 0)
    {
      ecma_object_t *key_p = ecma_get_object_from_value (key_arg);
      ecma_op_container_set_weak (key_p, map_object_p);
    }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) ||  ENABLED (JERRY_ES2015_BUILTIN_WEAKSET) */
  }
  else
  {
    ecma_op_internal_buffer_update (entry_p, ecma_op_container_set_noramlize_zero (value_arg), lit_id);
  }

  ecma_ref_object ((ecma_object_t *) map_object_p);
  return this_arg;
} /* ecma_op_container_set */