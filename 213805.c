ecma_op_container_entry_size (lit_magic_string_id_t lit_id) /**< class id */
{
  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL || lit_id == LIT_MAGIC_STRING_MAP_UL)
  {
    return ECMA_CONTAINER_PAIR_SIZE;
  }

  return ECMA_CONTAINER_VALUE_SIZE;
} /* ecma_op_container_entry_size */