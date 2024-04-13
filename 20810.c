static void test_too_big_image(void)
{
    RedMemSlotInfo mem_info;
    RedSurfaceCmd *cmd;
    QXLSurfaceCmd qxl;

    init_meminfo(&mem_info);
    init_qxl_surface(&qxl);

    /* try to create a surface quite large.
     * The sizes (width and height) were chosen so the multiplication
     * using 32 bit values gives a very small value.
     * These kind of values should be refused as they will cause
     * overflows. Also the total memory for the card is not enough to
     * hold the surface so surely can't be accepted.
     */
    qxl.u.surface_create.stride = 0x08000004 * 4;
    qxl.u.surface_create.width = 0x08000004;
    qxl.u.surface_create.height = 0x40000020;
    cmd = red_surface_cmd_new(NULL, &mem_info, 0, to_physical(&qxl));
    g_assert_null(cmd);

    deinit_qxl_surface(&qxl);
    memslot_info_destroy(&mem_info);
}