    yaffscache_chunk_find_insertion_point(YAFFSFS_INFO *yfs, uint32_t obj_id, TSK_OFF_T offset, uint32_t seq_number, YaffsCacheChunk **chunk)
{
    YaffsCacheChunk *curr, *prev;

    // Have we seen this obj_id? If not, add an entry for it
    if(yfs->chunkMap->find(obj_id) == yfs->chunkMap->end()){
        fflush(stderr);
        YaffsCacheChunkGroup chunkGroup;
        chunkGroup.cache_chunks_head = NULL;
        chunkGroup.cache_chunks_tail = NULL;
        yfs->chunkMap->insert(std::make_pair(obj_id, chunkGroup));
    }

    curr = yfs->chunkMap->operator[](obj_id).cache_chunks_head;
    prev = NULL;

    if (chunk == NULL) {
        return TSK_ERR;
    }

    while(curr != NULL) {
        // Compares obj id, then seq num, then offset. -1 => current < new
        int cmp = yaffscache_chunk_compare(curr, obj_id, offset, seq_number);

        if (cmp == 0) {
            *chunk = curr;
            return TSK_OK;
        }
        else if (cmp == 1) {
            *chunk = prev;
            return TSK_STOP;
        }

        prev = curr;
        curr = curr->ycc_next;
    }

    *chunk = prev;
    return TSK_STOP;
}