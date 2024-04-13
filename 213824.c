ecma_op_container_size (ecma_value_t this_arg, /**< this argument */
                        lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  return ecma_make_uint32_value (ECMA_CONTAINER_GET_SIZE (container_p));
} /* ecma_op_container_size */