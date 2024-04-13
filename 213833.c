ecma_op_container_free_map_entries (ecma_collection_t *container_p)
{
  JERRY_ASSERT (container_p != NULL);

  uint32_t entry_count = ECMA_CONTAINER_ENTRY_COUNT (container_p);
  ecma_value_t *start_p = ECMA_CONTAINER_START (container_p);

  for (uint32_t i = 0; i < entry_count; i += ECMA_CONTAINER_PAIR_SIZE)
  {
    ecma_container_pair_t *entry_p = (ecma_container_pair_t *) (start_p + i);

    if (ecma_is_value_empty (entry_p->key))
    {
      continue;
    }

    ecma_free_value_if_not_object (entry_p->key);
    ecma_free_value_if_not_object (entry_p->value);

    entry_p->key = ECMA_VALUE_EMPTY;
    entry_p->value = ECMA_VALUE_EMPTY;
  }
} /* ecma_op_container_free_map_entries */