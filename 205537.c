rsvg_filter_primitive_specular_lighting_render (RsvgNode *node, RsvgFilterPrimitive *primitive, RsvgFilterContext *ctx)
{
    RsvgFilterPrimitiveSpecularLighting *specular_lighting = (RsvgFilterPrimitiveSpecularLighting *) primitive;

    gint x, y;
    gdouble z, surfaceScale;
    gint rowstride, height, width;
    gdouble factor, max, base;
    vector3 lightcolor, color;
    vector3 L;
    cairo_matrix_t iaffine;
    RsvgIRect boundarys;
    RsvgNodeLightSource *source = NULL;

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

    color.x = ((guchar *) (&specular_lighting->lightingcolor))[2] / 255.0;
    color.y = ((guchar *) (&specular_lighting->lightingcolor))[1] / 255.0;
    color.z = ((guchar *) (&specular_lighting->lightingcolor))[0] / 255.0;

    surfaceScale = specular_lighting->surfaceScale / 255.0;

    for (y = boundarys.y0; y < boundarys.y1; y++)
        for (x = boundarys.x0; x < boundarys.x1; x++) {
            z = in_pixels[y * rowstride + x * 4 + 3] * surfaceScale;
            L = get_light_direction (source, x, y, z, &iaffine, ctx->ctx);
            L.z += 1;
            L = normalise (L);

            lightcolor = get_light_color (source, color, x, y, z, &iaffine, ctx->ctx);
            base = dotproduct (get_surface_normal (in_pixels, boundarys, x, y,
                                                   1, 1, 1.0 / ctx->paffine.xx,
                                                   1.0 / ctx->paffine.yy, specular_lighting->surfaceScale,
                                                   rowstride, ctx->channelmap[3]), L);

            factor = specular_lighting->specularConstant * pow (base, specular_lighting->specularExponent) * 255;

            max = 0;
            if (max < lightcolor.x)
                max = lightcolor.x;
            if (max < lightcolor.y)
                max = lightcolor.y;
            if (max < lightcolor.z)
                max = lightcolor.z;

            max *= factor;
            if (max > 255)
                max = 255;
            if (max < 0)
                max = 0;

            output_pixels[y * rowstride + x * 4 + ctx->channelmap[0]] = lightcolor.x * max;
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[1]] = lightcolor.y * max;
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[2]] = lightcolor.z * max;
            output_pixels[y * rowstride + x * 4 + ctx->channelmap[3]] = max;

        }

    cairo_surface_mark_dirty (output);

    rsvg_filter_store_result (primitive->result, output, ctx);

    cairo_surface_destroy (in);
    cairo_surface_destroy (output);
}