ecma_op_internal_buffer_find (ecma_collection_t *container_p, /**< internal container pointer */
                              ecma_value_t key_arg, /**< key argument */
                              lit_magic_string_id_t lit_id) /**< class id */
{
  JERRY_ASSERT (container_p != NULL);

  uint8_t entry_size = ecma_op_container_entry_size (lit_id);
  uint32_t entry_count = ECMA_CONTAINER_ENTRY_COUNT (container_p);
  ecma_value_t *start_p = ECMA_CONTAINER_START (container_p);

  for (uint32_t i = 0; i < entry_count; i += entry_size)
  {
    ecma_value_t *entry_p = start_p + i;

    if (ecma_op_same_value_zero (*entry_p, key_arg))
    {
      return entry_p;
    }
  }

  return NULL;
} /* ecma_op_internal_buffer_find */