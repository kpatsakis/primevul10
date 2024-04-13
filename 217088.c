zexecfile(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    check_type_access(*op, t_file, a_executable | a_read | a_execute);
    check_estack(4);            /* cleanup, file, finish, file */
    push_mark_estack(es_other, execfile_cleanup);
    *++esp = *op;
    push_op_estack(execfile_finish);
    return zexec(i_ctx_p);
}