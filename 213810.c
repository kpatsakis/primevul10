ecma_op_container_clear (ecma_value_t this_arg, /**< this argument */
                         lit_magic_string_id_t lit_id) /**< internal class id */
{
  ecma_extended_object_t *map_object_p = ecma_op_container_get_object (this_arg, lit_id);

  if (map_object_p == NULL)
  {
    return ECMA_VALUE_ERROR;
  }

  ecma_op_container_free_entries ((ecma_object_t *) map_object_p);

  return ECMA_VALUE_UNDEFINED;
} /* ecma_op_container_clear */