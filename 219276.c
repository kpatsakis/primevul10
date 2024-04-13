    yaffs_make_unlinked( YAFFSFS_INFO * yaffsfs, TSK_FS_FILE * a_fs_file )
{
    TSK_FS_FILE * fs_file = a_fs_file;

    if (tsk_verbose)
        tsk_fprintf(stderr, "yaffs_make_unlinked: Making virtual unlinked node\n");

    if (yaffs_make_directory(yaffsfs, fs_file, YAFFS_OBJECT_UNLINKED, YAFFS_OBJECT_UNLINKED_NAME))
        return 1;

    return 0;
}