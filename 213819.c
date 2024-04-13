ecma_op_container_free_weakmap_entries (ecma_object_t *object_p, /**< object pointer */
                                        ecma_collection_t *container_p) /**< internal buffer pointer */
{
  JERRY_ASSERT (object_p != NULL);
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

    ecma_op_container_unref_weak (ecma_get_object_from_value (entry_p->key), ecma_make_object_value (object_p));
    ecma_op_container_remove_weak_entry (object_p, entry_p->key);

    ecma_free_value_if_not_object (entry_p->value);

    entry_p->key = ECMA_VALUE_EMPTY;
    entry_p->value = ECMA_VALUE_EMPTY;
  }
} /* ecma_op_container_free_weakmap_entries */