zfilenamelistseparator(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;

    push(1);
    make_const_string(op, avm_foreign | a_readonly, 1,
                      (const byte *)&gp_file_name_list_separator);
    return 0;
}