    yaffscache_versions_insert_chunk(YAFFSFS_INFO *yfs, YaffsCacheChunk *chunk)
{
    YaffsCacheObject *obj;
    TSK_RETVAL_ENUM result;
    YaffsCacheVersion *version;

    // Building a list in yfs->cache_objects, sorted by obj_id
    result = yaffscache_object_find_or_add(yfs, chunk->ycc_obj_id, &obj);
    if (result != TSK_OK) {
        return TSK_ERR;
    }
    version = obj->yco_latest;

    /* First chunk in this object? */
    if (version == NULL) {
        yaffscache_object_add_version(obj, chunk);
    }
    else {
        /* Chunk in the same update? */
        if (chunk->ycc_seq_number == version->ycv_seq_number) {
            version->ycv_last_chunk = chunk;
            if ((chunk->ycc_chunk_id == 0) && (chunk->ycc_parent_id != YAFFS_OBJECT_UNLINKED) 
                &&(chunk->ycc_parent_id != YAFFS_OBJECT_DELETED)) {
                    version->ycv_header_chunk = chunk;
            }
            else if((chunk->ycc_chunk_id == 0) && (version->ycv_header_chunk == NULL)){
                version->ycv_header_chunk = chunk;
            }
        }
        // If there was no header for the last version, continue adding to it instead
        // of starting a new version. 
        else if(version->ycv_header_chunk == NULL){
            version->ycv_seq_number = chunk->ycc_seq_number;
            version->ycv_last_chunk = chunk;
            if ((chunk->ycc_chunk_id == 0) && (chunk->ycc_parent_id != YAFFS_OBJECT_UNLINKED) 
                &&(chunk->ycc_parent_id != YAFFS_OBJECT_DELETED)) {
                    version->ycv_header_chunk = chunk;
            }
            else if((chunk->ycc_chunk_id == 0) && (version->ycv_header_chunk == NULL)){
                version->ycv_header_chunk = chunk;
            }
        }
        else if(chunk->ycc_chunk_id == 0){   // Directories only have a header block
            // If we're looking at a new version of a directory where the previous version had the same name, 
            // leave everything in the same version. Multiple versions of the same directory aren't really giving us 
            // any information.
            YaffsHeader * newHeader;
            yaffsfs_read_header(yfs, &newHeader, chunk->ycc_offset);
            if((newHeader != NULL) && (newHeader->obj_type == YAFFS_TYPE_DIRECTORY)){
                // Read in the old header
                YaffsHeader * oldHeader;
                yaffsfs_read_header(yfs, &oldHeader, version->ycv_header_chunk->ycc_offset);
                if((oldHeader != NULL) && (oldHeader->obj_type == YAFFS_TYPE_DIRECTORY) &&
                    (0 == strncmp(oldHeader->name, newHeader->name, YAFFS_HEADER_NAME_LENGTH))){
                        version->ycv_seq_number = chunk->ycc_seq_number;
                        version->ycv_last_chunk = chunk;
                        version->ycv_header_chunk = chunk;
                }
                else{
                    // The older header either isn't a directory or it doesn't have the same name, so leave it
                    // as its own version
                    yaffscache_object_add_version(obj, chunk);
                }
            }
            else{
                //  Not a directory
                yaffscache_object_add_version(obj, chunk);
            }
        }
        else{
            //  Otherwise, add this chunk as the start of a new version
            yaffscache_object_add_version(obj, chunk);
        }
    }

    return TSK_OK;
}