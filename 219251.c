    yaffsfs_inode_walk(TSK_FS_INFO *fs, TSK_INUM_T start_inum,
    TSK_INUM_T end_inum, TSK_FS_META_FLAG_ENUM flags,
    TSK_FS_META_WALK_CB a_action, void *a_ptr)
{
    YAFFSFS_INFO *yfs = (YAFFSFS_INFO *)fs;
    TSK_FS_FILE *fs_file;
    TSK_RETVAL_ENUM result;

    uint32_t start_obj_id;
    uint32_t start_ver_number;
    uint32_t end_obj_id;
    uint32_t end_ver_number;

    uint32_t obj_id;

    YaffsCacheObject *curr_obj;
    YaffsCacheVersion *curr_version;

    result = yaffscache_inode_to_obj_id_and_version(start_inum, &start_obj_id, &start_ver_number);

    result = yaffscache_inode_to_obj_id_and_version(end_inum, &end_obj_id, &end_ver_number);

    if (end_obj_id < start_obj_id) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_WALK_RNG);
        tsk_error_set_errstr("yaffsfs_inode_walk: end object id must be >= start object id: "
            "%" PRIx32 " must be >= %" PRIx32 "",
            end_obj_id, start_obj_id);
        return 1;
    }

    /* The ORPHAN flag is unsupported for YAFFS2 */
    if (flags & TSK_FS_META_FLAG_ORPHAN) {
        if (tsk_verbose){
            tsk_fprintf(stderr, "yaffsfs_inode_walk: ORPHAN flag unsupported by YAFFS2");
        }
    }

    if (((flags & TSK_FS_META_FLAG_ALLOC) == 0) &&
        ((flags & TSK_FS_META_FLAG_UNALLOC) == 0)) {
            flags = (TSK_FS_META_FLAG_ENUM)(flags | TSK_FS_META_FLAG_ALLOC | TSK_FS_META_FLAG_UNALLOC);
    }

    /* If neither of the USED or UNUSED flags are set, then set them
    * both
    */
    if (((flags & TSK_FS_META_FLAG_USED) == 0) &&
        ((flags & TSK_FS_META_FLAG_UNUSED) == 0)) {
            flags = (TSK_FS_META_FLAG_ENUM)(flags | TSK_FS_META_FLAG_USED | TSK_FS_META_FLAG_UNUSED);
    }

    if ((fs_file = tsk_fs_file_alloc(fs)) == NULL)
        return 1;
    if ((fs_file->meta =
        tsk_fs_meta_alloc(YAFFS_FILE_CONTENT_LEN)) == NULL)
        return 1;


    for (obj_id = start_obj_id; obj_id <= end_obj_id; obj_id++) {
        int retval;

        result = yaffscache_version_find_by_inode(yfs, obj_id, &curr_version, &curr_obj);
        if (result == TSK_OK) {

            TSK_INUM_T curr_inode;
            YaffsCacheVersion *version;

            // ALLOC, UNALLOC, or both are set at this point 
            if (flags & TSK_FS_META_FLAG_ALLOC) {
                // Allocated only - just look at current version
                if (yaffscache_obj_id_and_version_to_inode(obj_id, curr_obj->yco_latest->ycv_version, &curr_inode) != TSK_OK) {
                    tsk_fs_file_close(fs_file);
                    return 1;
                }

                // It's possible for the current version to be unallocated if the last header was a deleted or unlinked header
                if(yaffs_is_version_allocated(yfs, curr_inode)){
                    if (yaffs_inode_lookup(fs, fs_file, curr_inode) != TSK_OK) {
                        tsk_fs_file_close(fs_file);
                        return 1;
                    }

                    retval = a_action(fs_file, a_ptr);
                    if (retval == TSK_WALK_STOP) {
                        tsk_fs_file_close(fs_file);
                        return 0;
                    }
                    else if (retval == TSK_WALK_ERROR) {
                        tsk_fs_file_close(fs_file);
                        return 1;
                    }
                }
            }
            if (flags & TSK_FS_META_FLAG_UNALLOC){
                for (version = curr_obj->yco_latest; version != NULL; version = version->ycv_prior) {
                    if (yaffscache_obj_id_and_version_to_inode(obj_id, version->ycv_version, &curr_inode) != TSK_OK) {
                        tsk_fs_file_close(fs_file);
                        return 1;
                    }

                    if(! yaffs_is_version_allocated(yfs, curr_inode)){
                        if (yaffs_inode_lookup(fs, fs_file, curr_inode) != TSK_OK) {
                            tsk_fs_file_close(fs_file);
                            return 1;
                        }

                        retval = a_action(fs_file, a_ptr);
                        if (retval == TSK_WALK_STOP) {
                            tsk_fs_file_close(fs_file);
                            return 0;
                        }
                        else if (retval == TSK_WALK_ERROR) {
                            tsk_fs_file_close(fs_file);
                            return 1;
                        }
                    }
                }
            }

            curr_obj = curr_obj->yco_next;
        }
    }

    /*
    * Cleanup.
    */
    tsk_fs_file_close(fs_file);
    return 0;
}