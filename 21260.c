gdk_pixbuf__xbm_image_load_real (FILE     *f, 
				 XBMData  *context, 
				 GError  **error)
{
	guint w, h;
	int x_hot, y_hot;
	guchar *data, *ptr;
	guchar *pixels;
	guint row_stride;
	int x, y;
	int reg = 0; /* Quiet compiler */
	int bits;

	GdkPixbuf *pixbuf;

	if (!read_bitmap_file_data (f, &w, &h, &data, &x_hot, &y_hot)) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                     _("Invalid XBM file"));
		return NULL;
	}

	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, w, h);

        if (pixbuf == NULL) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                     _("Insufficient memory to load XBM image file"));
                return NULL;
        }
        
	if (x_hot != -1 && y_hot != -1) {
		gchar hot[10];
		g_snprintf (hot, 10, "%d", x_hot);
		gdk_pixbuf_set_option (pixbuf, "x_hot", hot);
		g_snprintf (hot, 10, "%d", y_hot);
		gdk_pixbuf_set_option (pixbuf, "y_hot", hot);
	}

	pixels = gdk_pixbuf_get_pixels (pixbuf);
	row_stride = gdk_pixbuf_get_rowstride (pixbuf);

	if (context && context->prepare_func)
		(* context->prepare_func) (pixbuf, NULL, context->user_data);


	/* Initialize PIXBUF */

	ptr = data;
	for (y = 0; y < h; y++) {
		bits = 0;
		for (x = 0; x < w; x++) {
			guchar channel;
			if (bits == 0) {
				reg = *ptr++;
				bits = 8;
			}

			channel = (reg & 1) ? 0 : 255;
			reg >>= 1;
			bits--;

			pixels[x*3+0] = channel;
			pixels[x*3+1] = channel;
			pixels[x*3+2] = channel;
		}
		pixels += row_stride;
	}
	g_free (data);

	if (context) {
		if (context->update_func)
			(* context->update_func) (pixbuf, 0, 0, w, h, context->user_data);
	}

	return pixbuf;
}