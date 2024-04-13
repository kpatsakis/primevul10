rsvg_filter_primitive_diffuse_lighting_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveDiffuseLighting *diffuse_lighting = (RsvgFilterPrimitiveDiffuseLighting *) primitive;

    gint x, y;
    float dy, dx, rawdy, rawdx;
    gdouble z;
    gint rowstride, height, width;
    gdouble factor, surfaceScale;
    vector3 lightcolor, L, N;
    vector3 color;
    cairo_matrix_t iaffine;
    RsvgNodeLightSource *source = NULL;
    RsvgIRect boundarys;

    guchar *in_pixels;
    guchar *output_pixels;

    cairo_surface_t *output, *in;

    source = find_light_source_in_children (node);
    if (source == NULL)
        return;

    iaffine = ctx->paffine;
    if (cairo_matrix_invert (&iaffine) != CAIRO_STATUS_SUCCESS)
      return;

    boundarys = rsvg_filter_primitive_get_bounds (primitive, ctx);

    in = rsvg_filter_get_in (primitive->in, ctx);
    if (in == NULL)
        return;

    cairo_surface_flush (in);

    in_pixels = cairo_image_surface_get_data (in);

    height = cairo_image_surface_get_height (in);
    width = cairo_image_surface_get_width (in);

    rowstride = cairo_image_surface_get_stride (in);

    output = _rsvg_image_surface_new (width, height);
    if (output == NULL) {
        cairo_surface_destroy (in);
        return;
    }

    output_pixels = cairo_image_surface_get_data (output);

    color.x = ((guchar *) (&diffuse_lighting->lightingcolor))[2] / 255.0;
    color.y = ((guchar *) (&diffuse_lighting->lightingcolor))[1] / 255.0;
    color.z = ((guchar *) (&diffuse_lighting->lightingcolor))[0] / 255.0;

    surfaceScale = diffuse_lighting->surfaceScale / 255.0;

    if (diffuse_lighting->dy < 0 || diffuse_lighting->dx < 0) {
        dx = 1;
        dy = 1;
        rawdx = 1;
        rawdy = 1;
    } else {
        dx = diffuse_lighting->dx * ctx->paffine.xx;
        dy = diffuse_lighting->dy * ctx->paffine.yy;
        rawdx = diffuse_lighting->dx;
        rawdy = diffuse_lighting->dy;
    }

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            z = surfaceScale * (double) in_pixels[y * rowstride + x * 4 + ctx->channelmap[3]];
            L = get_light_direction (source, x, y, z, &iaffine, ctx->ctx);
            N = get_surface_normal (in_pixels, boundarys, x, y,
                                    dx, dy, rawdx, rawdy, diffuse_lighting->surfaceScale,
                                    rowstride, ctx->channelmap[3]);
            lightcolor = get_light_color (source, color, x, y, z, &iaffine, ctx->ctx);
            factor = dotproduct (N, L);

            output_pixels[y * rowstride + x * 4 + ctx->channelmap[0]] =
                MAX (0, MIN (255, diffuse_lighting->diffuseConstant * factor * lightcolor.x * 255.0));
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[1]] =
                MAX (0, MIN (255, diffuse_lighting->diffuseConstant * factor * lightcolor.y * 255.0));
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[2]] =
                MAX (0, MIN (255, diffuse_lighting->diffuseConstant * factor * lightcolor.z * 255.0));
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[3]] = 255;
        }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}