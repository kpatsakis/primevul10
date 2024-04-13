static int on_frame_send(h2_stream_state_t state, int frame_type)
{
    return on_frame(state, frame_type, trans_on_send, H2_ALEN(trans_on_send));
}