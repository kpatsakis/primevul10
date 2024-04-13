get_interp_pixel (guchar * src, gdouble ox, gdouble oy, guchar ch, RsvgIRect boundarys,
                  guint rowstride)
{
    double xmod, ymod;
    double dist1, dist2, dist3, dist4;
    double c, c1, c2, c3, c4;
    double fox, foy, cox, coy;

    xmod = fmod (ox, 1.0);
    ymod = fmod (oy, 1.0);

    dist1 = (1 - xmod) * (1 - ymod);
    dist2 = (xmod) * (1 - ymod);
    dist3 = (xmod) * (ymod);
    dist4 = (1 - xmod) * (ymod);

    fox = floor (ox);
    foy = floor (oy);
    cox = ceil (ox);
    coy = ceil (oy);

    if (fox <= boundarys.x0 || fox >= boundarys.x1 ||
        foy <= boundarys.y0 || foy >= boundarys.y1)
        c1 = 0;
    else
        c1 = src[(guint) foy * rowstride + (guint) fox * 4 + ch];

    if (cox <= boundarys.x0 || cox >= boundarys.x1 ||
        foy <= boundarys.y0 || foy >= boundarys.y1)
        c2 = 0;
    else
        c2 = src[(guint) foy * rowstride + (guint) cox * 4 + ch];

    if (cox <= boundarys.x0 || cox >= boundarys.x1 ||
        coy <= boundarys.y0 || coy >= boundarys.y1)
        c3 = 0;
    else
        c3 = src[(guint) coy * rowstride + (guint) cox * 4 + ch];

    if (fox <= boundarys.x0 || fox >= boundarys.x1 ||
        coy <= boundarys.y0 || coy >= boundarys.y1)
        c4 = 0;
    else
        c4 = src[(guint) coy * rowstride + (guint) fox * 4 + ch];

    c = (c1 * dist1 + c2 * dist2 + c3 * dist3 + c4 * dist4) / (dist1 + dist2 + dist3 + dist4);

    return (guchar) c;
}