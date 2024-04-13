    yaffsfs_dir_open_meta(TSK_FS_INFO *a_fs, TSK_FS_DIR ** a_fs_dir,
    TSK_INUM_T a_addr)
{
    TSK_FS_DIR *fs_dir;
    TSK_FS_NAME *fs_name;
    YAFFSFS_INFO *yfs = (YAFFSFS_INFO *)a_fs;
    int should_walk_children = 0;
    uint32_t obj_id;
    uint32_t ver_number;

    if (a_addr < a_fs->first_inum || a_addr > a_fs->last_inum) { 
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_WALK_RNG);
        tsk_error_set_errstr("yaffs_dir_open_meta: Invalid inode value: %"
            PRIuINUM, a_addr);
        return TSK_ERR;
    }
    else if (a_fs_dir == NULL) {
        tsk_error_reset();
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr("yaffs_dir_open_meta: NULL fs_dir argument given");
        return TSK_ERR;
    }

    fs_dir = *a_fs_dir;

    if (fs_dir) {
        tsk_fs_dir_reset(fs_dir);
        fs_dir->addr = a_addr;
    }
    else if ((*a_fs_dir = fs_dir = tsk_fs_dir_alloc(a_fs, a_addr, 128)) == NULL) {
        return TSK_ERR;
    }

    if (tsk_verbose)
        fprintf(stderr,"yaffs_dir_open_meta: called for directory %" PRIu32 "\n", (uint32_t) a_addr);

    //  handle the orphan directory if its contents were requested
    if (a_addr == TSK_FS_ORPHANDIR_INUM(a_fs)) {
        return tsk_fs_dir_find_orphans(a_fs, fs_dir);
    }

    if ((fs_name = tsk_fs_name_alloc(YAFFSFS_MAXNAMLEN, 0)) == NULL) {
        return TSK_ERR;
    }


    if ((fs_dir->fs_file = 
        tsk_fs_file_open_meta(a_fs, NULL, a_addr)) == NULL) {
            tsk_error_errstr2_concat(" - yaffs_dir_open_meta");
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
    }

    // extract obj_id and ver_number from inum
    yaffscache_inode_to_obj_id_and_version(a_addr, &obj_id, &ver_number);

    // Decide if we should walk the directory structure
    if (obj_id == YAFFS_OBJECT_DELETED ||
        obj_id == YAFFS_OBJECT_UNLINKED) {
            should_walk_children = 1;
    }
    else {
        YaffsCacheObject *obj;
        YaffsCacheVersion *versionFound;
        TSK_RETVAL_ENUM result = yaffscache_version_find_by_inode(yfs, a_addr, &versionFound, &obj);
        if (result != TSK_OK) {
            if (tsk_verbose)
                tsk_fprintf(stderr, "yaffsfs_dir_open_meta: yaffscache_version_find_by_inode failed! (inode: %d\n", a_addr);
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }

        /* Only attach files onto the latest version of the directory */
        should_walk_children = (obj->yco_latest == versionFound);
    }

    // Search the cache for the children of this object and add them to fs_dir
    if (should_walk_children) {
        dir_open_cb_args args;
        args.yfs = yfs;
        args.dir = fs_dir;
        args.parent_addr = a_addr;
        yaffscache_find_children(yfs, a_addr, yaffs_dir_open_meta_cb, &args);
    }

    // add special entries to root directory
    if (obj_id == YAFFS_OBJECT_ROOT) {
        strncpy(fs_name->name, YAFFS_OBJECT_UNLINKED_NAME, fs_name->name_size);
        fs_name->meta_addr = YAFFS_OBJECT_UNLINKED;
        fs_name->type = TSK_FS_NAME_TYPE_DIR;
        fs_name->flags = TSK_FS_NAME_FLAG_ALLOC;
        if (tsk_fs_dir_add(fs_dir, fs_name)) {
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }

        strncpy(fs_name->name, YAFFS_OBJECT_DELETED_NAME, fs_name->name_size);
        fs_name->meta_addr = YAFFS_OBJECT_DELETED;
        fs_name->type = TSK_FS_NAME_TYPE_DIR;
        fs_name->flags = TSK_FS_NAME_FLAG_ALLOC;
        if (tsk_fs_dir_add(fs_dir, fs_name)) {
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }

        // orphan directory
        if (tsk_fs_dir_make_orphan_dir_name(a_fs, fs_name)) {
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }
        fs_name->meta_addr = yfs->fs_info.last_inum;
        fs_name->type = TSK_FS_NAME_TYPE_DIR;
        fs_name->flags = TSK_FS_NAME_FLAG_ALLOC;
        if (tsk_fs_dir_add(fs_dir, fs_name)) {
            tsk_fs_name_free(fs_name);
            return TSK_ERR;
        }
    }

    tsk_fs_name_free(fs_name);
    return TSK_OK;
}