static void deinit_qxl_surface(QXLSurfaceCmd *qxl)
{
    g_free(from_physical(qxl->u.surface_create.data));
}