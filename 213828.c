ecma_op_internal_buffer_delete (ecma_collection_t *container_p, /**< internal container pointer */
                                ecma_container_pair_t *entry_p, /**< entry pointer */
                                lit_magic_string_id_t lit_id) /**< class id */
{
  JERRY_ASSERT (container_p != NULL);
  JERRY_ASSERT (entry_p != NULL);

  ecma_free_value_if_not_object (entry_p->key);
  entry_p->key = ECMA_VALUE_EMPTY;

  if (lit_id == LIT_MAGIC_STRING_WEAKMAP_UL || lit_id == LIT_MAGIC_STRING_MAP_UL)
  {
    ecma_free_value_if_not_object (entry_p->value);
    entry_p->value = ECMA_VALUE_EMPTY;
  }

  ECMA_CONTAINER_SET_SIZE (container_p, ECMA_CONTAINER_GET_SIZE (container_p) - 1);
} /* ecma_op_internal_buffer_delete */