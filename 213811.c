ecma_op_container_free_entries (ecma_object_t *object_p) /**< collection object pointer */
{
  JERRY_ASSERT (object_p != NULL);

  ecma_extended_object_t *map_object_p = (ecma_extended_object_t *) object_p;
  ecma_collection_t *container_p = ECMA_GET_INTERNAL_VALUE_POINTER (ecma_collection_t,
                                                                    map_object_p->u.class_prop.u.value);

  switch (map_object_p->u.class_prop.class_id)
  {
#if ENABLED (JERRY_ES2015_BUILTIN_WEAKSET)
    case LIT_MAGIC_STRING_WEAKSET_UL:
    {
      ecma_op_container_free_weakset_entries (object_p, container_p);
      break;
    }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKSET) */
#if ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP)
    case LIT_MAGIC_STRING_WEAKMAP_UL:
    {
      ecma_op_container_free_weakmap_entries (object_p, container_p);
      break;
    }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_WEAKMAP) */
#if ENABLED (JERRY_ES2015_BUILTIN_SET)
    case LIT_MAGIC_STRING_SET_UL:
    {
      ecma_op_container_free_set_entries (container_p);
      break;
    }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_SET) */
#if ENABLED (JERRY_ES2015_BUILTIN_MAP)
    case LIT_MAGIC_STRING_MAP_UL:
    {
      ecma_op_container_free_map_entries (container_p);
      break;
    }
#endif /* ENABLED (JERRY_ES2015_BUILTIN_MAP) */
    default:
    {
      break;
    }
  }

  ECMA_CONTAINER_SET_SIZE (container_p, 0);
} /* ecma_op_container_free_entries */