connection_discard_blank_line (chunkqueue * const cq, uint32_t header_len)
{
    /*(separate func only to be able to mark with compiler hint as cold)*/
    chunkqueue_mark_written(cq, header_len);
    return cq->first; /* refresh c after chunkqueue_mark_written() */
}