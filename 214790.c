static void iter_from_extension(pb_field_iter_t *iter, pb_extension_t *extension)
{
    /* Fake a field iterator for the extension field.
     * It is not actually safe to advance this iterator, but decode_field
     * will not even try to. */
    const pb_field_t *field = (const pb_field_t*)extension->type->arg;
    (void)pb_field_iter_begin(iter, field, extension->dest);
    iter->pData = extension->dest;
    iter->pSize = &extension->found;
    
    if (PB_ATYPE(field->type) == PB_ATYPE_POINTER)
    {
        /* For pointer extensions, the pointer is stored directly
         * in the extension structure. This avoids having an extra
         * indirection. */
        iter->pData = &extension->dest;
    }
}