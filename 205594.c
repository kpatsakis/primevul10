rsvg_filter_primitive_image_render_in (RsvgFilterPrimitiveImage *image, RsvgFilterContext * context)
{
    RsvgDrawingCtx *ctx;
    RsvgNode *drawable;
    cairo_surface_t *result;

    ctx = context->ctx;

    if (!image->href)
        return NULL;

    drawable = rsvg_drawing_ctx_acquire_node (ctx, image->href->str);
    if (!drawable)
        return NULL;

    rsvg_current_state (ctx)->affine = context->paffine;

    result = rsvg_get_surface_of_node (ctx, drawable, context->width, context->height);

    rsvg_drawing_ctx_release_node (ctx, drawable);

    return result;
}