    yaffsfs_load_attrs(TSK_FS_FILE *file)
{
    TSK_FS_ATTR *attr;
    TSK_FS_META *meta;
    TSK_FS_INFO *fs;
    YAFFSFS_INFO *yfs;
    TSK_FS_ATTR_RUN *data_run;
    TSK_DADDR_T file_block_count;
    YaffsCacheObject *obj;
    YaffsCacheVersion *version;
    TSK_RETVAL_ENUM result;
    TSK_LIST *chunks_seen = NULL;
    YaffsCacheChunk *curr;
    TSK_FS_ATTR_RUN *data_run_new;


    if (file == NULL || file->meta == NULL || file->fs_info == NULL)
    {
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr
            ("yaffsfs_load_attrs: called with NULL pointers");
        return 1;
    }

    meta = file->meta;
    yfs = (YAFFSFS_INFO *)file->fs_info;
    fs = &yfs->fs_info;

    // see if we have already loaded the runs
    if ((meta->attr != NULL)
        && (meta->attr_state == TSK_FS_META_ATTR_STUDIED)) {
            return 0;
    }
    else if (meta->attr_state == TSK_FS_META_ATTR_ERROR) {
        return 1;
    }
    // not sure why this would ever happen, but...
    else if (meta->attr != NULL) {
        tsk_fs_attrlist_markunused(meta->attr);
    }
    else if (meta->attr == NULL) {
        meta->attr = tsk_fs_attrlist_alloc();
    }

    attr = tsk_fs_attrlist_getnew(meta->attr, TSK_FS_ATTR_NONRES);
    if (attr == NULL) {
        meta->attr_state = TSK_FS_META_ATTR_ERROR;
        return 1;
    }

    if (meta->size == 0) {
        data_run = NULL;
    }
    else {
        /* BC: I'm not entirely sure this is needed.  My guess is that
         * this was done instead of maintaining the head of the list of 
         * runs.  In theory, the tsk_fs_attr_add_run() method should handle
         * the fillers. */
        data_run = tsk_fs_attr_run_alloc();
        if (data_run == NULL) {
            tsk_fs_attr_run_free(data_run);
            meta->attr_state = TSK_FS_META_ATTR_ERROR;
            return 1;
        }

        data_run->offset = 0;
        data_run->addr = 0;
        data_run->len = (meta->size + fs->block_size - 1) / fs->block_size;
        data_run->flags = TSK_FS_ATTR_RUN_FLAG_FILLER;
    }
    

    // initialize the data run
    if (tsk_fs_attr_set_run(file, attr, data_run, NULL,
        TSK_FS_ATTR_TYPE_DEFAULT, TSK_FS_ATTR_ID_DEFAULT,
        meta->size, meta->size, roundup(meta->size, fs->block_size), (TSK_FS_ATTR_FLAG_ENUM)0, 0)) {
            meta->attr_state = TSK_FS_META_ATTR_ERROR;
            return 1;
    }

    // If the file has size zero, return now
    if(meta->size == 0){
        meta->attr_state = TSK_FS_META_ATTR_STUDIED;
        return 0;
    }


    /* Get the version for the given object. */
    result = yaffscache_version_find_by_inode(yfs, meta->addr, &version, &obj);
    if (result != TSK_OK || version == NULL) {
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffsfs_load_attrs: yaffscache_version_find_by_inode failed!\n");
        meta->attr_state = TSK_FS_META_ATTR_ERROR;
        return 1;
    }

    if (tsk_verbose)
        yaffscache_object_dump(stderr, obj);

    file_block_count = data_run->len;
    /* Cycle through the chunks for this version of this object */
    curr = version->ycv_last_chunk;
    while (curr != NULL && curr->ycc_obj_id == obj->yco_obj_id) {

        if (curr->ycc_chunk_id == 0) {
            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffsfs_load_attrs: skipping header chunk\n");
        }
        else if (tsk_list_find(chunks_seen, curr->ycc_chunk_id)) {
            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffsfs_load_attrs: skipping duplicate chunk\n");
        }
        else if (curr->ycc_chunk_id > file_block_count) {
            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffsfs_load_attrs: skipping chunk past end\n");
        }
        /* We like this chunk */
        else {
            // add it to our internal list
            if (tsk_list_add(&chunks_seen, curr->ycc_chunk_id)) {
                meta->attr_state = TSK_FS_META_ATTR_ERROR;
                tsk_list_free(chunks_seen);
                chunks_seen = NULL;
                return 1;
            }

            data_run_new = tsk_fs_attr_run_alloc();
            if (data_run_new == NULL) {
                tsk_fs_attr_run_free(data_run_new);
                meta->attr_state = TSK_FS_META_ATTR_ERROR;
                return 1;
            }

            data_run_new->offset = (curr->ycc_chunk_id - 1);
            data_run_new->addr = curr->ycc_offset / (fs->block_pre_size + fs->block_size + fs->block_post_size);
            data_run_new->len = 1;
            data_run_new->flags = TSK_FS_ATTR_RUN_FLAG_NONE;

            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffsfs_load_attrs: @@@ Chunk %d : %08x is at offset 0x%016llx\n",
                curr->ycc_chunk_id, curr->ycc_seq_number, curr->ycc_offset);

            tsk_fs_attr_add_run(fs, attr, data_run_new);
        }

        curr = curr->ycc_prev;
    }

    tsk_list_free(chunks_seen);
    meta->attr_state = TSK_FS_META_ATTR_STUDIED;
    return 0;
}