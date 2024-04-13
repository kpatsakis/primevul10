static void vnc_update_throttle_offset(VncState *vs)
{
    size_t offset =
        vs->client_width * vs->client_height * vs->client_pf.bytes_per_pixel;

    if (vs->audio_cap) {
        int bps;
        switch (vs->as.fmt) {
        default:
        case  AUDIO_FORMAT_U8:
        case  AUDIO_FORMAT_S8:
            bps = 1;
            break;
        case  AUDIO_FORMAT_U16:
        case  AUDIO_FORMAT_S16:
            bps = 2;
            break;
        case  AUDIO_FORMAT_U32:
        case  AUDIO_FORMAT_S32:
            bps = 4;
            break;
        }
        offset += vs->as.freq * bps * vs->as.nchannels;
    }

    /* Put a floor of 1MB on offset, so that if we have a large pending
     * buffer and the display is resized to a small size & back again
     * we don't suddenly apply a tiny send limit
     */
    offset = MAX(offset, 1024 * 1024);

    if (vs->throttle_output_offset != offset) {
        trace_vnc_client_throttle_threshold(
            vs, vs->ioc, vs->throttle_output_offset, offset, vs->client_width,
            vs->client_height, vs->client_pf.bytes_per_pixel, vs->audio_cap);
    }

    vs->throttle_output_offset = offset;
}