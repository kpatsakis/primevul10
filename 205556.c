rsvg_filter_primitive_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    primitive->render (node, primitive, ctx);
}