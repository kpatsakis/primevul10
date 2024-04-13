static void qxl_unpack_chunks(void *dest, size_t size, PCIQXLDevice *qxl,
                              QXLDataChunk *chunk, uint32_t group_id)
{
    uint32_t max_chunks = 32;
    size_t offset = 0;
    size_t bytes;

    for (;;) {
        bytes = MIN(size - offset, chunk->data_size);
        memcpy(dest + offset, chunk->data, bytes);
        offset += bytes;
        if (offset == size) {
            return;
        }
        chunk = qxl_phys2virt(qxl, chunk->next_chunk, group_id);
        if (!chunk) {
            return;
        }
        max_chunks--;
        if (max_chunks == 0) {
            return;
        }
    }
}