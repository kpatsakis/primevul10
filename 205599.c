rsvg_art_affine_image (cairo_surface_t *img,
                       cairo_surface_t *intermediate,
                       cairo_matrix_t *affine,
                       double w,
                       double h)
{
    cairo_matrix_t inv_affine, raw_inv_affine;
    gint intstride;
    gint basestride;
    gint basex, basey;
    gdouble fbasex, fbasey;
    gdouble rawx, rawy;
    guchar *intpix;
    guchar *basepix;
    gint i, j, k, basebpp, ii, jj;
    gboolean has_alpha;
    gdouble pixsum[4];
    gboolean xrunnoff, yrunnoff;
    gint iwidth, iheight;
    gint width, height;

    g_assert (cairo_image_surface_get_format (intermediate) == CAIRO_FORMAT_ARGB32);

    cairo_surface_flush (img);

    width = cairo_image_surface_get_width (img);
    height = cairo_image_surface_get_height (img);
    iwidth = cairo_image_surface_get_width (intermediate);
    iheight = cairo_image_surface_get_height (intermediate);

    has_alpha = cairo_image_surface_get_format (img) == CAIRO_FORMAT_ARGB32;

    basestride = cairo_image_surface_get_stride (img);
    intstride = cairo_image_surface_get_stride (intermediate);
    basepix = cairo_image_surface_get_data (img);
    intpix = cairo_image_surface_get_data (intermediate);
    basebpp = has_alpha ? 4 : 3;

    raw_inv_affine = *affine;
    if (cairo_matrix_invert (&raw_inv_affine) != CAIRO_STATUS_SUCCESS)
      return FALSE;

    cairo_matrix_init_scale (&inv_affine, w, h);
    cairo_matrix_multiply (&inv_affine, &inv_affine, affine);
    if (cairo_matrix_invert (&inv_affine) != CAIRO_STATUS_SUCCESS)
      return FALSE;

    /*apply the transformation */
    for (i = 0; i < iwidth; i++)
        for (j = 0; j < iheight; j++) {
            fbasex = (inv_affine.xx * (double) i + inv_affine.xy * (double) j +
                      inv_affine.x0) * (double) width;
            fbasey = (inv_affine.yx * (double) i + inv_affine.yy * (double) j +
                      inv_affine.y0) * (double) height;
            basex = floor (fbasex);
            basey = floor (fbasey);
            rawx = raw_inv_affine.xx * i + raw_inv_affine.xy * j + raw_inv_affine.x0;
            rawy = raw_inv_affine.yx * i + raw_inv_affine.yy * j + raw_inv_affine.y0;
            if (rawx < 0 || rawy < 0 || rawx >= w ||
                rawy >= h || basex < 0 || basey < 0 || basex >= width || basey >= height) {
                for (k = 0; k < 4; k++)
                    intpix[i * 4 + j * intstride + k] = 0;
            } else {
                if (basex < 0 || basex + 1 >= width)
                    xrunnoff = TRUE;
                else
                    xrunnoff = FALSE;
                if (basey < 0 || basey + 1 >= height)
                    yrunnoff = TRUE;
                else
                    yrunnoff = FALSE;
                for (k = 0; k < basebpp; k++)
                    pixsum[k] = 0;
                for (ii = 0; ii < 2; ii++)
                    for (jj = 0; jj < 2; jj++) {
                        if (basex + ii < 0 || basey + jj < 0
                            || basex + ii >= width || basey + jj >= height);
                        else {
                            for (k = 0; k < basebpp; k++) {
                                pixsum[k] +=
                                    (double) basepix[basebpp * (basex + ii) +
                                                     (basey + jj) * basestride + k]
                                    * (xrunnoff ? 1 : fabs (fbasex - (double) (basex + (1 - ii))))
                                    * (yrunnoff ? 1 : fabs (fbasey - (double) (basey + (1 - jj))));
                            }
                        }
                    }
                for (k = 0; k < basebpp; k++)
                    intpix[i * 4 + j * intstride + k] = pixsum[k];
                if (!has_alpha)
                    intpix[i * 4 + j * intstride + 3] = 255;
            }

        }

    /* Don't need cairo_surface_mark_dirty(intermediate) here since
     * the only caller does further work and then calls that himself.
     */

    return TRUE;
}