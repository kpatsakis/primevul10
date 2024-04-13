    yaffs_inode_lookup(TSK_FS_INFO *a_fs, TSK_FS_FILE * a_fs_file,
    TSK_INUM_T inum)
{
    YAFFSFS_INFO *yfs = (YAFFSFS_INFO *)a_fs;
    YaffsCacheObject *obj;
    YaffsCacheVersion *version;
    YaffsHeader *header = NULL;
    YaffsSpare *spare = NULL;
    TSK_RETVAL_ENUM result;
    uint8_t type;
    const char *real_name;

    if (a_fs_file == NULL) {
        tsk_error_set_errno(TSK_ERR_FS_ARG);
        tsk_error_set_errstr("yaffsfs_inode_lookup: fs_file is NULL");
        return 1;
    }

    if (a_fs_file->meta == NULL) {
        if ((a_fs_file->meta =
            tsk_fs_meta_alloc(YAFFS_FILE_CONTENT_LEN)) == NULL)
            return 1;
    }
    else {
        tsk_fs_meta_reset(a_fs_file->meta);
    }

    if (tsk_verbose)
        tsk_fprintf(stderr, "yaffs_inode_lookup: looking up %" PRIuINUM "\n",inum);

    switch(inum) {
    case YAFFS_OBJECT_UNLINKED:
        yaffs_make_unlinked(yfs, a_fs_file);
        return 0;

    case YAFFS_OBJECT_DELETED:
        yaffs_make_deleted(yfs, a_fs_file);
        return 0;
    }

    if(inum == yfs->fs_info.last_inum){
        yaffs_make_orphan_dir(yfs, a_fs_file);
        return 0;
    }

    result = yaffscache_version_find_by_inode(yfs, inum, &version, &obj);
    if (result != TSK_OK) {
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: yaffscache_version_find_by_inode failed! (inode = %d)\n", inum);
        return 1;
    }

    if(version->ycv_header_chunk == NULL){
        return 1;
    }

    if (yaffsfs_read_chunk(yfs, &header, &spare, version->ycv_header_chunk->ycc_offset) != TSK_OK) {
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: yaffsfs_read_chunk failed!\n");
        return 1;
    }

    type = header->obj_type;

    switch(inum) {
    case YAFFS_OBJECT_LOSTNFOUND:
        real_name = YAFFS_OBJECT_LOSTNFOUND_NAME;
        break;
    case YAFFS_OBJECT_UNLINKED:
        real_name = YAFFS_OBJECT_UNLINKED_NAME;
        break;
    case YAFFS_OBJECT_DELETED:
        real_name = YAFFS_OBJECT_DELETED_NAME;
        break;
    default:
        real_name = header->name;
        break;
    }

    switch(type) {
    case YAFFS_TYPE_FILE:
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: is a file\n");
        yaffs_make_regularfile(yfs, a_fs_file, inum, real_name);
        break;

    case YAFFS_TYPE_DIRECTORY:
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: is a directory\n");
        yaffs_make_directory(yfs, a_fs_file, inum, real_name);
        break;

    case YAFFS_TYPE_SOFTLINK:
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: is a symbolic link\n");
        yaffs_make_regularfile(yfs, a_fs_file, inum, real_name);
        a_fs_file->meta->type = TSK_FS_META_TYPE_LNK;
        break;

    case YAFFS_TYPE_HARDLINK:
    case YAFFS_TYPE_UNKNOWN:
    default:
        if (tsk_verbose)
            tsk_fprintf(stderr, "yaffs_inode_lookup: is *** UNHANDLED *** (type %d, header at 0x%x)\n", type, version->ycv_header_chunk->ycc_offset);
        // We can still set a few things
        a_fs_file->meta->type = TSK_FS_META_TYPE_UNDEF;
        a_fs_file->meta->addr = inum;
        if(yaffs_is_version_allocated(yfs, inum)){
            a_fs_file->meta->flags =
                (TSK_FS_META_FLAG_ENUM)(TSK_FS_META_FLAG_USED | TSK_FS_META_FLAG_ALLOC);
        }
        else{
            a_fs_file->meta->flags =
                (TSK_FS_META_FLAG_ENUM)(TSK_FS_META_FLAG_USED | TSK_FS_META_FLAG_UNALLOC);
        }
        if (a_fs_file->meta->name2 == NULL) {
            if ((a_fs_file->meta->name2 = (TSK_FS_META_NAME_LIST *)
                tsk_malloc(sizeof(TSK_FS_META_NAME_LIST))) == NULL){
                    return 1;
            }
            a_fs_file->meta->name2->next = NULL;
        }
        strncpy(a_fs_file->meta->name2->name, real_name,
            TSK_FS_META_NAME_LIST_NSIZE);
        break;
    }

    /* Who owns this? I'm following the way FATFS does it by freeing + NULLing 
    * this and mallocing if used. 
    */
    free(a_fs_file->meta->link);
    a_fs_file->meta->link = NULL;

    if (type != YAFFS_TYPE_HARDLINK) {
        a_fs_file->meta->mode = (TSK_FS_META_MODE_ENUM)(header->file_mode & TWELVE_BITS_MASK); // chop at 12 bits;
        a_fs_file->meta->uid = header->user_id;
        a_fs_file->meta->gid = header->group_id;
        a_fs_file->meta->mtime = header->mtime;
        a_fs_file->meta->atime = header->atime;
        a_fs_file->meta->ctime = header->ctime;
    }

    if (type == YAFFS_TYPE_FILE) {
        a_fs_file->meta->size = header->file_size;
        // NOTE: This isn't in Android 3.3 kernel but is in YAFFS2 git
        //a_fs_file->meta->size |= ((TSK_OFF_T) header->file_size_high) << 32;
    }

    if (type == YAFFS_TYPE_HARDLINK) {
        // TODO: Store equivalent_id somewhere? */
    }

    if (type == YAFFS_TYPE_SOFTLINK) {
        a_fs_file->meta->link = (char*)tsk_malloc(YAFFS_HEADER_ALIAS_LENGTH);
        if (a_fs_file->meta->link == NULL) {
            free(header);
            free(spare);
            return 1;
        }

        memcpy(a_fs_file->meta->link, header->alias, YAFFS_HEADER_ALIAS_LENGTH);
    }

    free(header);
    free(spare);
    return 0;
}