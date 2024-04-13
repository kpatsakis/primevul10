static void test_circular_small_chunks(void)
{
    RedMemSlotInfo mem_info;
    RedCursorCmd *red_cursor_cmd;
    QXLCursorCmd cursor_cmd;
    QXLCursor *cursor;
    QXLDataChunk *chunks[2];

    init_meminfo(&mem_info);
    g_test_expect_message(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
                          "*red_get_data_chunks_ptr: data split in too many chunks, avoiding DoS*");

    /* a circular list of small chunks should not be a problems */
    memset(&cursor_cmd, 0, sizeof(cursor_cmd));
    cursor_cmd.type = QXL_CURSOR_SET;

    cursor = create_chunk(SPICE_OFFSETOF(QXLCursor, chunk), 1, NULL, 0xaa);
    cursor->header.unique = 1;
    cursor->header.width = 128;
    cursor->header.height = 128;
    cursor->data_size = 128 * 128 * 4;

    chunks[0] = create_chunk(0, 1, &cursor->chunk, 0xaa);
    chunks[0]->next_chunk = to_physical(&cursor->chunk);

    cursor_cmd.u.set.shape = to_physical(cursor);

    red_cursor_cmd = red_cursor_cmd_new(NULL, &mem_info, 0, to_physical(&cursor_cmd));
    if (red_cursor_cmd != NULL) {
        /* function does not return errors so there should be no data */
        g_assert_cmpuint(red_cursor_cmd->type, ==, QXL_CURSOR_SET);
        g_assert_cmpuint(red_cursor_cmd->u.set.position.x, ==, 0);
        g_assert_cmpuint(red_cursor_cmd->u.set.position.y, ==, 0);
        g_assert_cmpuint(red_cursor_cmd->u.set.shape.data_size, ==, 0);
        red_cursor_cmd_unref(red_cursor_cmd);
    }
    g_test_assert_expected_messages();

    g_free(cursor);
    g_free(chunks[0]);
    memslot_info_destroy(&mem_info);
}