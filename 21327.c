static void key_event(VncState *vs, int down, uint32_t sym)
{
    int keycode;
    int lsym = sym;

    if (lsym >= 'A' && lsym <= 'Z' && qemu_console_is_graphic(NULL)) {
        lsym = lsym - 'A' + 'a';
    }

    keycode = keysym2scancode(vs->vd->kbd_layout, lsym & 0xFFFF,
                              vs->vd->kbd, down) & SCANCODE_KEYMASK;
    trace_vnc_key_event_map(down, sym, keycode, code2name(keycode));
    do_key_event(vs, down, keycode, sym);
}