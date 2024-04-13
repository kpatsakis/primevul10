get_light_direction (RsvgNodeLightSource * source, gdouble x1, gdouble y1, gdouble z,
                     cairo_matrix_t *affine, RsvgDrawingCtx * ctx)
{
    vector3 output;

    switch (source->type) {
    case DISTANTLIGHT:
        output.x = cos (source->azimuth) * cos (source->elevation);
        output.y = sin (source->azimuth) * cos (source->elevation);
        output.z = sin (source->elevation);
        break;
    default:
        {
            double x, y;
            x = affine->xx * x1 + affine->xy * y1 + affine->x0;
            y = affine->yx * x1 + affine->yy * y1 + affine->y0;
            output.x = rsvg_length_normalize (&source->x, ctx) - x;
            output.y = rsvg_length_normalize (&source->y, ctx) - y;
            output.z = rsvg_length_normalize (&source->z, ctx) - z;
            output = normalise (output);
        }
        break;
    }
    return output;
}