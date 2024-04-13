ecma_op_internal_buffer_update (ecma_value_t *entry_p, /**< entry pointer */
                                ecma_value_t value_arg, /**< value argument */
                                lit_magic_string_id_t lit_id) /**< class id */
{
  JERRY_ASSERT (entry_p != NULL);

  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL || lit_id == LIT_MAGIC_STRING_MAP_UL)
  {
    ecma_free_value_if_not_object (((ecma_container_pair_t *) entry_p)->value);

    ((ecma_container_pair_t *) entry_p)->value = ecma_copy_value_if_not_object (value_arg);
  }
} /* ecma_op_internal_buffer_update */