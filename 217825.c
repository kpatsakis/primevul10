encode_SET_QUEUE(const struct ofpact_queue *queue,
                 enum ofp_version ofp_version, struct ofpbuf *out)
{
    put_OFPAT_SET_QUEUE(out, ofp_version, queue->queue_id);
}