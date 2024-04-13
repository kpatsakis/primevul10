    yaffscache_object_add_version(YaffsCacheObject *obj, YaffsCacheChunk *chunk)
{
    uint32_t ver_number;
    YaffsCacheChunk *header_chunk = NULL;
    YaffsCacheVersion *version;

    // Going to try ignoring unlinked/deleted headers (objID 3 and 4)
    if ((chunk->ycc_chunk_id == 0) && (chunk->ycc_parent_id != YAFFS_OBJECT_UNLINKED) 
        &&(chunk->ycc_parent_id != YAFFS_OBJECT_DELETED)) {
            header_chunk = chunk;
    }

    /* If this is the second version (since last header_chunk is not NULL) and no
    * header was added, get rid of this incomplete old version -- can't be
    * reasonably recovered.
    *
    * TODO: These chunks are still in the structure and can be walked,
    *       but I'm not sure how to represent this set of data chunks
    *       with no metadata under TSK. This is rare and we don't have
    *       a testcase for it now. Punting right now.
    *
    * Edit: Shouldn't get to this point anymore. Changes to 
    *       yaffscache_versions_insert_chunk make a version continue until it
    *       has a header block.
    */
    if (obj->yco_latest != NULL) {
        if (obj->yco_latest->ycv_header_chunk == NULL) {
            YaffsCacheVersion *incomplete = obj->yco_latest;

            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffscache_object_add_version: "
                "removed an incomplete first version (no header)\n");

            obj->yco_latest = obj->yco_latest->ycv_prior;
            free(incomplete);
        }
    }

    if (obj->yco_latest != NULL) {
        ver_number = obj->yco_latest->ycv_version + 1;

        /* Until a new header is given, use the last seen header. */
        if (header_chunk == NULL) {
            header_chunk = obj->yco_latest->ycv_header_chunk;

            // If we haven't seen a good header yet and we have a deleted/unlinked one, use it
            if((header_chunk == NULL) && (chunk->ycc_chunk_id == 0)){
                header_chunk = chunk;
            }
        }
    }
    else {
        ver_number = 1;
    }

    if ((version = (YaffsCacheVersion *) tsk_malloc(sizeof(YaffsCacheVersion))) == NULL) {
        return TSK_ERR;
    }

    version->ycv_prior = obj->yco_latest;
    version->ycv_version = ver_number;
    version->ycv_seq_number = chunk->ycc_seq_number;
    version->ycv_header_chunk = header_chunk;
    version->ycv_first_chunk = chunk;
    version->ycv_last_chunk = chunk;

    obj->yco_latest = version;

    return TSK_OK;
}