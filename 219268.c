    yaffsfs_jblk_walk(TSK_FS_INFO * /*info*/, TSK_DADDR_T /*daddr*/,
    TSK_DADDR_T /*daddrt*/, int /*entry*/, TSK_FS_JBLK_WALK_CB /*cb*/,
    void * /*fn*/)
{
    tsk_error_reset();
    tsk_error_set_errno(TSK_ERR_FS_UNSUPFUNC);
    tsk_error_set_errstr("Journal support for YAFFS is not implemented");
    return 1;
}