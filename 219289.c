    yaffsfs_jentry_walk(TSK_FS_INFO * /*info*/, int /*entry*/,
    TSK_FS_JENTRY_WALK_CB /*cb*/, void * /*fn*/)
{
    tsk_error_reset();
    tsk_error_set_errno(TSK_ERR_FS_UNSUPFUNC);
    tsk_error_set_errstr("Journal support for YAFFS is not implemented");
    return 1;
}