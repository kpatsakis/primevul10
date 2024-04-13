ecma_op_create_internal_buffer (void)
{
  ecma_collection_t *collection_p = ecma_new_collection ();
  ecma_collection_push_back (collection_p, (ecma_value_t) 0);

  return collection_p;
} /* ecma_op_create_internal_buffer */