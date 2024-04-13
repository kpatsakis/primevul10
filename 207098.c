static void qxl_render_update_area_unlocked(PCIQXLDevice *qxl)
{
    VGACommonState *vga = &qxl->vga;
    DisplaySurface *surface;
    int width = qxl->guest_head0_width ?: qxl->guest_primary.surface.width;
    int height = qxl->guest_head0_height ?: qxl->guest_primary.surface.height;
    int i;

    if (qxl->guest_primary.resized) {
        qxl->guest_primary.resized = 0;
        qxl->guest_primary.data = qxl_phys2virt(qxl,
                                                qxl->guest_primary.surface.mem,
                                                MEMSLOT_GROUP_GUEST);
        if (!qxl->guest_primary.data) {
            goto end;
        }
        qxl_set_rect_to_surface(qxl, &qxl->dirty[0]);
        qxl->num_dirty_rects = 1;
        trace_qxl_render_guest_primary_resized(
               width,
               height,
               qxl->guest_primary.qxl_stride,
               qxl->guest_primary.bytes_pp,
               qxl->guest_primary.bits_pp);
        if (qxl->guest_primary.qxl_stride > 0) {
            pixman_format_code_t format =
                qemu_default_pixman_format(qxl->guest_primary.bits_pp, true);
            surface = qemu_create_displaysurface_from
                (width,
                 height,
                 format,
                 qxl->guest_primary.abs_stride,
                 qxl->guest_primary.data);
        } else {
            surface = qemu_create_displaysurface
                (width,
                 height);
        }
        dpy_gfx_replace_surface(vga->con, surface);
    }

    if (!qxl->guest_primary.data) {
        goto end;
    }
    for (i = 0; i < qxl->num_dirty_rects; i++) {
        if (qemu_spice_rect_is_empty(qxl->dirty+i)) {
            break;
        }
        if (qxl->dirty[i].left < 0 ||
            qxl->dirty[i].top < 0 ||
            qxl->dirty[i].left > qxl->dirty[i].right ||
            qxl->dirty[i].top > qxl->dirty[i].bottom ||
            qxl->dirty[i].right > width ||
            qxl->dirty[i].bottom > height) {
            continue;
        }
        qxl_blit(qxl, qxl->dirty+i);
        dpy_gfx_update(vga->con,
                       qxl->dirty[i].left, qxl->dirty[i].top,
                       qxl->dirty[i].right - qxl->dirty[i].left,
                       qxl->dirty[i].bottom - qxl->dirty[i].top);
    }
    qxl->num_dirty_rects = 0;

end:
    if (qxl->render_update_cookie_num == 0) {
        graphic_hw_update_done(qxl->ssd.dcl.con);
    }
}