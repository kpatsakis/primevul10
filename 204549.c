_rsvg_node_poly_draw (RsvgNode * self, RsvgDrawingCtx * ctx, int dominate)
{
    RsvgNodePoly *poly = (RsvgNodePoly *) self;

    if (poly->path == NULL)
        return;

    rsvg_state_reinherit_top (ctx, self->state, dominate);

    rsvg_render_path (ctx, poly->path);
}