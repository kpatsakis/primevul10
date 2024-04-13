    yaffs_make_regularfile( YAFFSFS_INFO * yaffsfs, TSK_FS_FILE * a_fs_file, 
    TSK_INUM_T inode, const char * name )
{
    TSK_FS_FILE *fs_file = a_fs_file;

    fs_file->meta->type = TSK_FS_META_TYPE_REG;
    fs_file->meta->mode = (TSK_FS_META_MODE_ENUM)0;
    fs_file->meta->nlink =1;

    if(yaffs_is_version_allocated(yaffsfs, inode)){
        fs_file->meta->flags =
            (TSK_FS_META_FLAG_ENUM)(TSK_FS_META_FLAG_USED | TSK_FS_META_FLAG_ALLOC);
    }
    else{
        fs_file->meta->flags =
            (TSK_FS_META_FLAG_ENUM)(TSK_FS_META_FLAG_USED | TSK_FS_META_FLAG_UNALLOC);
    }

    fs_file->meta->uid = fs_file->meta->gid = 0;
    fs_file->meta->mtime = fs_file->meta->atime = fs_file->meta->ctime =
        fs_file->meta->crtime = 0;
    fs_file->meta->mtime_nano = fs_file->meta->atime_nano =
        fs_file->meta->ctime_nano = fs_file->meta->crtime_nano = 0;

    if (fs_file->meta->name2 == NULL) {
        if ((fs_file->meta->name2 = (TSK_FS_META_NAME_LIST *)
            tsk_malloc(sizeof(TSK_FS_META_NAME_LIST))) == NULL)
            return 1;
        fs_file->meta->name2->next = NULL;
    }

    if (fs_file->meta->attr != NULL) {
        tsk_fs_attrlist_markunused(fs_file->meta->attr);
    }
    else {
        fs_file->meta->attr = tsk_fs_attrlist_alloc();
    }

    fs_file->meta->addr = inode;
    strncpy(fs_file->meta->name2->name, name,
        TSK_FS_META_NAME_LIST_NSIZE);

    fs_file->meta->size = 0;
    fs_file->meta->attr_state = TSK_FS_META_ATTR_EMPTY;

    return 0;
}