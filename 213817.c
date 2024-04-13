ecma_op_container_get_value (ecma_value_t *entry_p, /**< entry (key) pointer */
                             lit_magic_string_id_t lit_id) /**< class id */
{
  JERRY_ASSERT (entry_p != NULL);

  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL || lit_id == LIT_MAGIC_STRING_MAP_UL)
  {
    return ((ecma_container_pair_t *) entry_p)->value;
  }

  return *entry_p;
} /* ecma_op_container_get_value */