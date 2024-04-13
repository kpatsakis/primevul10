static void audio_add(VncState *vs)
{
    struct audio_capture_ops ops;

    if (vs->audio_cap) {
        error_report("audio already running");
        return;
    }

    ops.notify = audio_capture_notify;
    ops.destroy = audio_capture_destroy;
    ops.capture = audio_capture;

    vs->audio_cap = AUD_add_capture(vs->vd->audio_state, &vs->as, &ops, vs);
    if (!vs->audio_cap) {
        error_report("Failed to add audio capture");
    }
}