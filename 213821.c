ecma_op_container_delete_weak (ecma_value_t this_arg, /**< this argument */
                               ecma_value_t key_arg, /**< key argument */
                               lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  if (!ecma_is_value_object (key_arg))
  {
    return ECMA_VALUE_FALSE;
  }

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  ecma_value_t *entry_p = ecma_op_internal_buffer_find (container_p, key_arg, lit_id);

  if (entry_p == NULL)
  {
    return ECMA_VALUE_FALSE;
  }

  ecma_op_internal_buffer_delete (container_p, (ecma_container_pair_t *) entry_p, lit_id);

  ecma_object_t *key_object_p = ecma_get_object_from_value (key_arg);
  ecma_op_container_unref_weak (key_object_p, ecma_make_object_value ((ecma_object_t *) map_object_p));

  return ECMA_VALUE_TRUE;
} /* ecma_op_container_delete_weak */