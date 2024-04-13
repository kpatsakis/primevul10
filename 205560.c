rsvg_filter_primitive_get_bounds (RsvgFilterPrimitive * self, RsvgFilterContext * ctx)
{
    RsvgBbox box, otherbox;
    cairo_matrix_t affine;

    cairo_matrix_init_identity (&affine);
    rsvg_bbox_init (&box, &affine);
    rsvg_bbox_init (&otherbox, &ctx->affine);
    otherbox.virgin = 0;
    if (ctx->filter->filterunits == objectBoundingBox)
        rsvg_drawing_ctx_push_view_box (ctx->ctx, 1., 1.);
    otherbox.rect.x = rsvg_length_normalize (&ctx->filter->x, ctx->ctx);
    otherbox.rect.y = rsvg_length_normalize (&ctx->filter->y, ctx->ctx);
    otherbox.rect.width = rsvg_length_normalize (&ctx->filter->width, ctx->ctx);
    otherbox.rect.height = rsvg_length_normalize (&ctx->filter->height, ctx->ctx);
    if (ctx->filter->filterunits == objectBoundingBox)
        rsvg_drawing_ctx_pop_view_box (ctx->ctx);

    rsvg_bbox_insert (&box, &otherbox);

    if (self != NULL) {
        if (self->x_specified || self->y_specified || self->width_specified || self->height_specified) {
            rsvg_bbox_init (&otherbox, &ctx->paffine);
            otherbox.virgin = 0;
            if (ctx->filter->primitiveunits == objectBoundingBox)
                rsvg_drawing_ctx_push_view_box (ctx->ctx, 1., 1.);
            if (self->x_specified)
                otherbox.rect.x = rsvg_length_normalize (&self->x, ctx->ctx);
            else
                otherbox.rect.x = 0;
            if (self->y_specified)
                otherbox.rect.y = rsvg_length_normalize (&self->y, ctx->ctx);
            else
                otherbox.rect.y = 0;

            if (self->width_specified || self->height_specified) {
                double curr_vbox_w, curr_vbox_h;

                rsvg_drawing_ctx_get_view_box_size (ctx->ctx, &curr_vbox_w, &curr_vbox_h);

                if (self->width_specified)
                    otherbox.rect.width = rsvg_length_normalize (&self->width, ctx->ctx);
                else
                    otherbox.rect.width = curr_vbox_w;

                if (self->height_specified)
                    otherbox.rect.height = rsvg_length_normalize (&self->height, ctx->ctx);
                else
                    otherbox.rect.height = curr_vbox_h;
            }

            if (ctx->filter->primitiveunits == objectBoundingBox)
                rsvg_drawing_ctx_pop_view_box (ctx->ctx);
            rsvg_bbox_clip (&box, &otherbox);
        }
    }

    rsvg_bbox_init (&otherbox, &affine);
    otherbox.virgin = 0;
    otherbox.rect.x = 0;
    otherbox.rect.y = 0;
    otherbox.rect.width = ctx->width;
    otherbox.rect.height = ctx->height;
    rsvg_bbox_clip (&box, &otherbox);
    {
        RsvgIRect output = { box.rect.x, box.rect.y,
            box.rect.x + box.rect.width,
            box.rect.y + box.rect.height
        };
        return output;
    }
}