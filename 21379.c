static bool vnc_should_update(VncState *vs)
{
    switch (vs->update) {
    case VNC_STATE_UPDATE_NONE:
        break;
    case VNC_STATE_UPDATE_INCREMENTAL:
        /* Only allow incremental updates if the pending send queue
         * is less than the permitted threshold, and the job worker
         * is completely idle.
         */
        if (vs->output.offset < vs->throttle_output_offset &&
            vs->job_update == VNC_STATE_UPDATE_NONE) {
            return true;
        }
        trace_vnc_client_throttle_incremental(
            vs, vs->ioc, vs->job_update, vs->output.offset);
        break;
    case VNC_STATE_UPDATE_FORCE:
        /* Only allow forced updates if the pending send queue
         * does not contain a previous forced update, and the
         * job worker is completely idle.
         *
         * Note this means we'll queue a forced update, even if
         * the output buffer size is otherwise over the throttle
         * output limit.
         */
        if (vs->force_update_offset == 0 &&
            vs->job_update == VNC_STATE_UPDATE_NONE) {
            return true;
        }
        trace_vnc_client_throttle_forced(
            vs, vs->ioc, vs->job_update, vs->force_update_offset);
        break;
    }
    return false;
}