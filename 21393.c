static void vnc_abort_display_jobs(VncDisplay *vd)
{
    VncState *vs;

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_lock_output(vs);
        vs->abort = true;
        vnc_unlock_output(vs);
    }
    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_jobs_join(vs);
    }
    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_lock_output(vs);
        if (vs->update == VNC_STATE_UPDATE_NONE &&
            vs->job_update != VNC_STATE_UPDATE_NONE) {
            /* job aborted before completion */
            vs->update = vs->job_update;
            vs->job_update = VNC_STATE_UPDATE_NONE;
        }
        vs->abort = false;
        vnc_unlock_output(vs);
    }
}