    yaffscache_versions_compute(YAFFSFS_INFO *yfs)
{
    std::map<unsigned int,YaffsCacheChunkGroup>::iterator iter;
    for( iter = yfs->chunkMap->begin(); iter != yfs->chunkMap->end(); ++iter ) {
        YaffsCacheChunk *chunk_curr = yfs->chunkMap->operator[](iter->first).cache_chunks_head;

        while(chunk_curr != NULL) {
            if (yaffscache_versions_insert_chunk(yfs, chunk_curr) != TSK_OK) {
                return TSK_ERR;
            }

            chunk_curr = chunk_curr->ycc_next;
        }
    }

    return TSK_OK;
}