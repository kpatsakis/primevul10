    yaffscache_object_dump(FILE *fp, YaffsCacheObject *obj)
{
    YaffsCacheVersion *next_version = obj->yco_latest;
    YaffsCacheChunk *chunk = next_version->ycv_last_chunk;

    fprintf(fp, "Object %d\n", obj->yco_obj_id);
    while(chunk != NULL && chunk->ycc_obj_id == obj->yco_obj_id) {
        if (next_version != NULL && 
            chunk == next_version->ycv_last_chunk) {
                fprintf(fp, "  @%d: %p %p %p\n", 
                    next_version->ycv_version, 
                    (void*) next_version->ycv_header_chunk,
                    (void*) next_version->ycv_first_chunk,
                    (void*)next_version->ycv_last_chunk);
                next_version = next_version->ycv_prior;
        }

        fprintf(fp, "    + %p %08x %08x %0" PRIxOFF "\n",
            (void*) chunk,
            chunk->ycc_chunk_id,
            chunk->ycc_seq_number,
            chunk->ycc_offset);

        chunk = chunk->ycc_prev;
    }
}