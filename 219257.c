    yaffscache_chunks_free(YAFFSFS_INFO *yfs)
{
    if((yfs != NULL) && (yfs->chunkMap != NULL)){
        // Free the YaffsCacheChunks in each ChunkGroup
        std::map<unsigned int,YaffsCacheChunkGroup>::iterator iter;
        for( iter = yfs->chunkMap->begin(); iter != yfs->chunkMap->end(); ++iter ) {
            YaffsCacheChunk *chunk = yfs->chunkMap->operator[](iter->first).cache_chunks_head;
            while(chunk != NULL) {
                YaffsCacheChunk *to_free = chunk;
                chunk = chunk->ycc_next;
                free(to_free);
            }
        }

        // Free the map
        yfs->chunkMap->clear();
        delete yfs->chunkMap;
    }

}