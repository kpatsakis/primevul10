static void init_qxl_surface(QXLSurfaceCmd *qxl)
{
    void *surface_mem;

    memset(qxl, 0, sizeof(*qxl));

    qxl->surface_id = 123;

    qxl->u.surface_create.format = SPICE_SURFACE_FMT_32_xRGB;
    qxl->u.surface_create.width = 128;
    qxl->u.surface_create.stride = 512;
    qxl->u.surface_create.height = 128;
    surface_mem = g_malloc(0x10000);
    qxl->u.surface_create.data = to_physical(surface_mem);
}