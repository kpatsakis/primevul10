gremlin_flood_clients(struct multi_context *m)
{
    const int level = GREMLIN_PACKET_FLOOD_LEVEL(m->top.options.gremlin);
    if (level)
    {
        struct gc_arena gc = gc_new();
        struct buffer buf = alloc_buf_gc(BUF_SIZE(&m->top.c2.frame), &gc);
        struct packet_flood_parms parm = get_packet_flood_parms(level);
        int i;

        ASSERT(buf_init(&buf, FRAME_HEADROOM(&m->top.c2.frame)));
        parm.packet_size = min_int(parm.packet_size, MAX_RW_SIZE_TUN(&m->top.c2.frame));

        msg(D_GREMLIN, "GREMLIN_FLOOD_CLIENTS: flooding clients with %d packets of size %d",
            parm.n_packets,
            parm.packet_size);

        for (i = 0; i < parm.packet_size; ++i)
        {
            ASSERT(buf_write_u8(&buf, get_random() & 0xFF));
        }

        for (i = 0; i < parm.n_packets; ++i)
        {
            multi_bcast(m, &buf, NULL, NULL, 0);
        }

        gc_free(&gc);
    }
}