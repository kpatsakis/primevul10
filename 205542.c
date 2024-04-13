get_light_color (RsvgNodeLightSource * source, vector3 color,
                 gdouble x1, gdouble y1, gdouble z, cairo_matrix_t *affine, RsvgDrawingCtx * ctx)
{
    double base, angle, x, y;
    vector3 s;
    vector3 L;
    vector3 output;
    double sx, sy, sz, spx, spy, spz;

    if (source->type != SPOTLIGHT)
        return color;

    sx = rsvg_length_normalize (&source->x, ctx);
    sy = rsvg_length_normalize (&source->y, ctx);
    sz = rsvg_length_normalize (&source->z, ctx);
    spx = rsvg_length_normalize (&source->pointsAtX, ctx);
    spy = rsvg_length_normalize (&source->pointsAtY, ctx);
    spz = rsvg_length_normalize (&source->pointsAtZ, ctx);

    x = affine->xx * x1 + affine->xy * y1 + affine->x0;
    y = affine->yx * x1 + affine->yy * y1 + affine->y0;

    L.x = sx - x;
    L.y = sy - y;
    L.z = sz - z;
    L = normalise (L);

    s.x = spx - sx;
    s.y = spy - sy;
    s.z = spz - sz;
    s = normalise (s);

    base = -dotproduct (L, s);

    angle = acos (base);

    if (base < 0 || angle > source->limitingconeAngle) {
        output.x = 0;
        output.y = 0;
        output.z = 0;
        return output;
    }

    output.x = color.x * pow (base, source->specularExponent);
    output.y = color.y * pow (base, source->specularExponent);
    output.z = color.z * pow (base, source->specularExponent);

    return output;
}