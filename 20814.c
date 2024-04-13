static void test_stride_too_small(void)
{
    RedMemSlotInfo mem_info;
    RedSurfaceCmd *cmd;
    QXLSurfaceCmd qxl;

    init_meminfo(&mem_info);
    init_qxl_surface(&qxl);

    /* try to create a surface with a stride too small to fit
     * the entire width.
     * This can be used to cause buffer overflows so refuse it.
     */
    qxl.u.surface_create.stride = 256;
    cmd = red_surface_cmd_new(NULL, &mem_info, 0, to_physical(&qxl));
    g_assert_null(cmd);

    deinit_qxl_surface(&qxl);
    memslot_info_destroy(&mem_info);
}