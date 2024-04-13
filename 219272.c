    yaffsfs_jopen(TSK_FS_INFO * /*info*/, TSK_INUM_T /*inum*/)
{
    tsk_error_reset();
    tsk_error_set_errno(TSK_ERR_FS_UNSUPFUNC);
    tsk_error_set_errstr("Journal support for YAFFS is not implemented");
    return 1;
}