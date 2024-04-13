    yaffsfs_parse_image_load_cache(YAFFSFS_INFO * yfs)
{
    uint8_t status = TSK_OK;
    uint32_t nentries = 0;
    YaffsSpare *spare = NULL;

    uint8_t tempBuf[8];
    uint32_t parentID;

    if (yfs->cache_objects)
        return 0;

    for(TSK_OFF_T offset = 0;offset < yfs->fs_info.img_info->size;offset += yfs->page_size + yfs->spare_size){
        status = yaffsfs_read_spare( yfs, &spare, offset + yfs->page_size);
        if (status != TSK_OK) {
            break;
        }

        if (yaffsfs_is_spare_valid(yfs, spare) == TSK_OK) {


            if((spare->has_extra_fields) || (spare->chunk_id != 0)){
                yaffscache_chunk_add(yfs,
                    offset, 
                    spare->seq_number, 
                    spare->object_id, 
                    spare->chunk_id, 
                    spare->extra_parent_id);
            }
            else{
                // If we have a header block and didn't extract it already from the spare, get the parent ID from
                // the non-spare data
                if(8 == tsk_img_read(yfs->fs_info.img_info, offset, (char*) tempBuf, 8)){
                    memcpy(&parentID, &tempBuf[4], 4);

                    yaffscache_chunk_add(yfs,
                        offset, 
                        spare->seq_number, 
                        spare->object_id, 
                        spare->chunk_id, 
                        parentID);
                }
                else{
                    // Really shouldn't happen
                    fprintf(stderr, "Error reading header to get parent id at offset %" PRIxOFF "\n", offset);
                    yaffscache_chunk_add(yfs,
                        offset, 
                        spare->seq_number, 
                        spare->object_id, 
                        spare->chunk_id, 
                        0);
                }
            }
        }

        free(spare);
        spare = NULL;

        ++nentries;
    }

    if (tsk_verbose)
        fprintf(stderr, "yaffsfs_parse_image_load_cache: read %d entries\n", nentries);

    if (tsk_verbose)
        fprintf(stderr, "yaffsfs_parse_image_load_cache: started processing chunks for version cache...\n");
    fflush(stderr);

    // At this point, we have a list of chunks sorted by obj id, seq number, and offset
    // This makes the list of objects in cache_objects, which link to different versions
    yaffscache_versions_compute(yfs);

    if (tsk_verbose)
        fprintf(stderr, "yaffsfs_parse_image_load_cache: done version cache!\n");
    fflush(stderr);


    // Having multiple inodes point to the same object seems to cause trouble in TSK, especially in orphan file detection,
    //  so set the version number of the final one to zero.
    // While we're at it, find the highest obj_id and the highest version (before resetting to zero)
    YaffsCacheObject * currObj = yfs->cache_objects;
    YaffsCacheVersion * currVer;
    while(currObj != NULL){
        if(currObj->yco_obj_id > yfs->max_obj_id){
            yfs->max_obj_id = currObj->yco_obj_id;
        }

        currVer = currObj->yco_latest;
        if(currVer->ycv_version > yfs->max_version){
            yfs->max_version = currVer->ycv_version;
        }

        currVer->ycv_version = 0;
        currObj = currObj->yco_next;
    }

    // Use the max object id and version number to construct an upper bound on the inode
    TSK_INUM_T max_inum = 0;
    if (TSK_OK != yaffscache_obj_id_and_version_to_inode(yfs->max_obj_id, yfs->max_version, &max_inum)) {
        return TSK_ERR;
    }
    yfs->fs_info.last_inum = max_inum + 1; // One more for the orphan dir

    // Make sure the orphan dir is greater than the root dir
    if (yfs->fs_info.last_inum <= yfs->fs_info.root_inum) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS);
        tsk_error_set_errstr(
            "yaffsfs_parse_image_load_cache: Maximum inum %" PRIuINUM " is not greater than the root inum", yfs->fs_info.last_inum);
        return TSK_ERR;
    }

    return TSK_OK;
}