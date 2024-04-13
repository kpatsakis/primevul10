    yaffscache_chunk_add(YAFFSFS_INFO *yfs, TSK_OFF_T offset, uint32_t seq_number,
    uint32_t obj_id, uint32_t chunk_id, uint32_t parent_id)
{
    TSK_RETVAL_ENUM result;
    YaffsCacheChunk *prev;
    YaffsCacheChunk *chunk;
    if ((chunk = (YaffsCacheChunk*)tsk_malloc(sizeof(YaffsCacheChunk))) == NULL) {
        return TSK_ERR;
    }

    chunk->ycc_offset = offset;
    chunk->ycc_seq_number = seq_number;
    chunk->ycc_obj_id = obj_id;
    chunk->ycc_chunk_id = chunk_id;
    chunk->ycc_parent_id = parent_id;

    // Bit of a hack here. In some images, the root directory (obj_id = 1) lists iself as its parent
    // directory, which can cause issues later when we get directory contents. To prevent this,
    // if a chunk comes in with obj_id = 1 and parent_id = 1, manually set the parent ID to zero.
    if((obj_id == 1) && (parent_id == 1)){
        chunk->ycc_parent_id = 0;
    }

    // Find the chunk that should go right before the new chunk
    result = yaffscache_chunk_find_insertion_point(yfs, obj_id, offset, seq_number, &prev);

    if (result == TSK_ERR) {
        return TSK_ERR;
    }

    if (prev == NULL) {
        // No previous chunk - new chunk is the lowest we've seen and the new start of the list
        chunk->ycc_prev = NULL;
        chunk->ycc_next = yfs->chunkMap->operator[](obj_id).cache_chunks_head;
    }
    else {
        chunk->ycc_prev = prev;
        chunk->ycc_next = prev->ycc_next;
    }

    if (chunk->ycc_next != NULL) {
        // If we're not at the end, set the prev pointer on the next chunk to point to our new one
        chunk->ycc_next->ycc_prev = chunk;
    }
    else {
        yfs->chunkMap->operator[](obj_id).cache_chunks_tail = chunk;
    }

    if (chunk->ycc_prev != NULL) {
        // If we're not at the beginning, set the next pointer on the previous chunk to point at our new one
        chunk->ycc_prev->ycc_next = chunk;
    }
    else {
        yfs->chunkMap->operator[](obj_id).cache_chunks_head = chunk;
    }

    return TSK_OK;
}