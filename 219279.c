    yaffs_make_deleted( YAFFSFS_INFO * yaffsfs, TSK_FS_FILE * a_fs_file )
{
    TSK_FS_FILE *fs_file = a_fs_file;

    if (tsk_verbose)
        tsk_fprintf(stderr, "yaffs_make_deleted: Making virtual deleted node\n");

    if (yaffs_make_directory(yaffsfs, fs_file, YAFFS_OBJECT_DELETED, YAFFS_OBJECT_DELETED_NAME))
        return 1;

    return 0;
}