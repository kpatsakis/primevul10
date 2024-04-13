connection_handle_read_post_cq_compact (chunkqueue * const cq)
{
    /* combine first mem chunk with next non-empty mem chunk
     * (loop if next chunk is empty) */
    chunk *c = cq->first;
    if (NULL == c) return 0;
    const uint32_t mlen = buffer_string_length(c->mem) - (size_t)c->offset;
    while ((c = c->next)) {
        const uint32_t blen = buffer_string_length(c->mem) - (size_t)c->offset;
        if (0 == blen) continue;
        chunkqueue_compact_mem(cq, mlen + blen);
        return 1;
    }
    return 0;
}