static void audio_capture_notify(void *opaque, audcnotification_e cmd)
{
    VncState *vs = opaque;

    assert(vs->magic == VNC_MAGIC);
    switch (cmd) {
    case AUD_CNOTIFY_DISABLE:
        vnc_lock_output(vs);
        vnc_write_u8(vs, VNC_MSG_SERVER_QEMU);
        vnc_write_u8(vs, VNC_MSG_SERVER_QEMU_AUDIO);
        vnc_write_u16(vs, VNC_MSG_SERVER_QEMU_AUDIO_END);
        vnc_unlock_output(vs);
        vnc_flush(vs);
        break;

    case AUD_CNOTIFY_ENABLE:
        vnc_lock_output(vs);
        vnc_write_u8(vs, VNC_MSG_SERVER_QEMU);
        vnc_write_u8(vs, VNC_MSG_SERVER_QEMU_AUDIO);
        vnc_write_u16(vs, VNC_MSG_SERVER_QEMU_AUDIO_BEGIN);
        vnc_unlock_output(vs);
        vnc_flush(vs);
        break;
    }
}