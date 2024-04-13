    print_addr_act(YAFFSFS_INFO * /*fs_file*/, TSK_OFF_T /*a_off*/,
    TSK_DADDR_T addr, char * /*buf*/, size_t /*size*/,
    TSK_FS_BLOCK_FLAG_ENUM flags, void *a_ptr)
{
    YAFFSFS_PRINT_ADDR *print = (YAFFSFS_PRINT_ADDR *) a_ptr;

    if (flags & TSK_FS_BLOCK_FLAG_CONT) {
        tsk_fprintf(print->hFile, "%" PRIuDADDR " ", addr);

        if (++(print->idx) == 8) {
            tsk_fprintf(print->hFile, "\n");
            print->idx = 0;
        }
    }

    return TSK_WALK_CONT;
}