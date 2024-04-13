ecma_op_container_remove_weak_entry (ecma_object_t *object_p, /**< internal container object */
                                     ecma_value_t key_arg) /**< key */
{
  ecma_extended_object_t *map_object_p = (ecma_extended_object_t *) object_p;

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  ecma_value_t *entry_p = ecma_op_internal_buffer_find (container_p, key_arg, map_object_p->u.class_prop.class_id);

  JERRY_ASSERT (entry_p != NULL);

  ecma_op_internal_buffer_delete (container_p, (ecma_container_pair_t *) entry_p, map_object_p->u.class_prop.class_id);
} /* ecma_op_container_remove_weak_entry */