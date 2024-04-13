    yaffs_make_orphan_dir( YAFFSFS_INFO * yaffsfs, TSK_FS_FILE * a_fs_file )
{
    TSK_FS_FILE * fs_file = a_fs_file;
    TSK_FS_NAME *fs_name = tsk_fs_name_alloc(256, 0);
    if (fs_name == NULL)
        return TSK_ERR;

    if (tsk_verbose)
        tsk_fprintf(stderr, "yaffs_make_orphan_dir: Making orphan dir node\n");

    if (tsk_fs_dir_make_orphan_dir_name(&(yaffsfs->fs_info), fs_name)) {
        tsk_fs_name_free(fs_name);
        return TSK_ERR;
    }

    if (yaffs_make_directory(yaffsfs, fs_file, yaffsfs->fs_info.last_inum, (char *)fs_name)){
        tsk_fs_name_free(fs_name);
        return 1;
    }
    tsk_fs_name_free(fs_name);
    return 0;
}