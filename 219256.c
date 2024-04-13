    yaffsfs_fscheck(TSK_FS_INFO * /*fs*/, FILE * /*hFile*/)
{
    tsk_error_reset();
    tsk_error_set_errno(TSK_ERR_FS_UNSUPFUNC);
    tsk_error_set_errstr("fscheck not implemented yet for YAFFS");
    return 1;
}