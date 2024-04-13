rsvg_filter_primitive_gaussian_blur_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveGaussianBlur *gaussian = (RsvgFilterPrimitiveGaussianBlur *) primitive;

    int width, height;
    cairo_surface_t *output, *in;
    RsvgIRect boundarys;
    gdouble sdx, sdy;
    RsvgFilterPrimitiveOutput op;
    cairo_t *cr;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    op = rsvg_filter_get_result (primitive->in, ctx);
    in = op.surface;

    width = cairo_image_surface_get_width (in);
    height = cairo_image_surface_get_height (in);

    output = _rsvg_image_surface_new (width, height);

    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    /* scale the SD values */
    sdx = fabs (gaussian->sdx * ctx->paffine.xx);
    sdy = fabs (gaussian->sdy * ctx->paffine.yy);

    gaussian_blur_surface (in, output, sdx, sdy);

    /* Hard-clip to the filter area */
    if (!(boundarys.x0 == 0
          && boundarys.y0 == 0
          && boundarys.x1 == width
          && boundarys.y1 == height)) {
        cr = cairo_create (output);
        cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
        cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
        cairo_rectangle (cr, 0, 0, width, height);
        cairo_rectangle (cr,
                         boundarys.x0, boundarys.y0,
                         boundarys.x1 - boundarys.x0, boundarys.y1 - boundarys.y0);
        cairo_fill (cr);
        cairo_destroy (cr);
    }

    op.surface = output;
    op.bounds = boundarys;
    rsvg_filter_store_output (primitive->result, op, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}