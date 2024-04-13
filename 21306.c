static void press_key(VncState *vs, QKeyCode qcode)
{
    qkbd_state_key_event(vs->vd->kbd, qcode, true);
    qkbd_state_key_event(vs->vd->kbd, qcode, false);
}