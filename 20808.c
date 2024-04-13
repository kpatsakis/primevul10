static void test_cursor_command(void)
{
    RedMemSlotInfo mem_info;
    RedCursorCmd *red_cursor_cmd;
    QXLCursorCmd cursor_cmd;
    QXLCursor *cursor;

    init_meminfo(&mem_info);

    /* test base cursor with no problems */
    memset(&cursor_cmd, 0, sizeof(cursor_cmd));
    cursor_cmd.type = QXL_CURSOR_SET;

    cursor = create_chunk(SPICE_OFFSETOF(QXLCursor, chunk), 128 * 128 * 4, NULL, 0xaa);
    cursor->header.unique = 1;
    cursor->header.width = 128;
    cursor->header.height = 128;
    cursor->data_size = 128 * 128 * 4;

    cursor_cmd.u.set.shape = to_physical(cursor);

    red_cursor_cmd = red_cursor_cmd_new(NULL, &mem_info, 0, to_physical(&cursor_cmd));
    g_assert_nonnull(red_cursor_cmd);
    red_cursor_cmd_unref(red_cursor_cmd);
    g_free(cursor);
    memslot_info_destroy(&mem_info);
}