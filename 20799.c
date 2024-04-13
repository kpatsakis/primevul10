create_chunk(size_t prefix, uint32_t size, QXLDataChunk* prev, int fill)
{
    uint8_t *ptr = g_malloc0(prefix + sizeof(QXLDataChunk) + size);
    QXLDataChunk *qxl = (QXLDataChunk *) (ptr + prefix);
    memset(&qxl->data[0], fill, size);
    qxl->data_size = size;
    qxl->prev_chunk = to_physical(prev);
    if (prev)
        prev->next_chunk = to_physical(qxl);
    return ptr;
}