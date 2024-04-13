    yaffsfs_block_getflags(TSK_FS_INFO *fs, TSK_DADDR_T a_addr)
{
    YAFFSFS_INFO *yfs = (YAFFSFS_INFO *)fs;
    TSK_FS_BLOCK_FLAG_ENUM flags = TSK_FS_BLOCK_FLAG_UNUSED;

    TSK_OFF_T offset = (a_addr * (fs->block_pre_size + fs->block_size + fs->block_post_size)) + yfs->page_size;
    YaffsSpare *spare = NULL;
    YaffsHeader *header = NULL;
    if (yaffsfs_read_spare(yfs, &spare, offset) != TSK_OK) {
        /* NOTE: Uh, how do we signal error? */
        return flags;
    }

    if (yaffsfs_is_spare_valid(yfs, spare) == TSK_OK) {
        /* XXX: Do we count blocks of older versions unallocated?
        *      If so, we need a smarter way to do this :/
        *
        *      Walk the object from this block and see if this
        *      block is used in the latest version. Could pre-
        *      calculate this at cache time as well.
        */


        if (spare->chunk_id == 0) {
            flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_META);
        } else {
            flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_CONT);
        }

        // Have obj id and offset
        // 1. Is the current version of this object allocated?
        // 2. If this is a header, is it the header of the current version?
        // 3. Is the chunk id too big given the current header?
        // 4. Is there a more recent version of this chunk id?
        YaffsCacheObject * obj = NULL;
        yaffscache_object_find(yfs, spare->object_id, &obj);

        // The result really shouldn't be NULL since we loaded every chunk
        if(obj != NULL){
            if(! yaffs_is_version_allocated(yfs, spare->object_id)){
                // If the current version isn't allocated, then no chunks in it are
                flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNALLOC);
            }
            else if (obj->yco_latest == NULL || obj->yco_latest->ycv_header_chunk == NULL) { 
                flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNALLOC); 
            }
            else if(spare->chunk_id == 0){
                if(obj->yco_latest->ycv_header_chunk->ycc_offset == offset - yfs->page_size){
                    // Have header chunk and it's the most recent header chunk
                    flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_ALLOC);
                }
                else{
                    // Have header chunk but isn't the most recent
                    flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNALLOC);
                }
            }
            else{
                // Read in the full header
                yaffsfs_read_header(yfs, &header, obj->yco_latest->ycv_header_chunk->ycc_offset);

                // chunk_id is 1-based, so for example chunk id 2 would be too big for a file
                //   500 bytes long
                if(header->file_size <= ((spare->chunk_id - 1) * (fs->block_size))){
                    flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNALLOC);
                }
                else{
                    // Since at this point we know there should be a chunk with this chunk id in the file, if
                    // this is the most recent version of the chunk assume it's part of the current version of the object.
                    YaffsCacheChunk * curr = obj->yco_latest->ycv_last_chunk;
                    while(curr != NULL){ // curr should really never make it to the beginning of the list

                        // Did we find our chunk?
                        if(curr->ycc_offset == offset - yfs->page_size){
                            flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_ALLOC);
                            break;
                        }

                        // Did we find a different chunk with our chunk id?
                        if(curr->ycc_chunk_id == spare->chunk_id){
                            flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNALLOC);
                            break;
                        }
                        curr = curr->ycc_prev;
                    }
                }
            }
        }

    } else {
        flags = (TSK_FS_BLOCK_FLAG_ENUM)(flags | TSK_FS_BLOCK_FLAG_UNUSED | TSK_FS_BLOCK_FLAG_UNALLOC);
    }

    free(spare);
    free(header);
    return flags;
}