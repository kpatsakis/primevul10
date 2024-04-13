static void qxl_set_rect_to_surface(PCIQXLDevice *qxl, QXLRect *area)
{
    area->left   = 0;
    area->right  = qxl->guest_primary.surface.width;
    area->top    = 0;
    area->bottom = qxl->guest_primary.surface.height;
}