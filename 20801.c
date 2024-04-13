static void test_no_issues(void)
{
    RedMemSlotInfo mem_info;
    RedSurfaceCmd *cmd;
    QXLSurfaceCmd qxl;

    init_meminfo(&mem_info);
    init_qxl_surface(&qxl);

    /* try to create a surface with no issues, should succeed */
    cmd = red_surface_cmd_new(NULL, &mem_info, 0, to_physical(&qxl));
    g_assert_nonnull(cmd);
    red_surface_cmd_unref(cmd);

    deinit_qxl_surface(&qxl);
    memslot_info_destroy(&mem_info);
}