static int on_frame(h2_stream_state_t state, int frame_type, 
                    int frame_map[][H2_SS_MAX], apr_size_t maxlen)
{
    ap_assert(frame_type >= 0);
    ap_assert(state >= 0);
    if (frame_type >= maxlen) {
        return state; /* NOP, ignore unknown frame types */
    }
    return on_map(state, frame_map[frame_type]);
}