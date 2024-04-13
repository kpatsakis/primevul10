box_blur_line (gint box_width, gint even_offset,
               guchar *src, guchar *dest,
               gint len, gint bpp)
{
    gint  i;
    gint  lead;    /* This marks the leading edge of the kernel              */
    gint  output;  /* This marks the center of the kernel                    */
    gint  trail;   /* This marks the pixel BEHIND the last 1 in the
                      kernel; it's the pixel to remove from the accumulator. */
    gint  *ac;     /* Accumulator for each channel                           */

    g_assert (box_width > 0);

    ac = g_new0 (gint, bpp);

    /* The algorithm differs for even and odd-sized kernels.
     * With the output at the center,
     * If odd, the kernel might look like this: 0011100
     * If even, the kernel will either be centered on the boundary between
     * the output and its left neighbor, or on the boundary between the
     * output and its right neighbor, depending on even_lr.
     * So it might be 0111100 or 0011110, where output is on the center
     * of these arrays.
     */
    lead = 0;

    if (box_width % 2 != 0) {
        /* Odd-width kernel */
        output = lead - (box_width - 1) / 2;
        trail  = lead - box_width;
    } else {
        /* Even-width kernel. */
        if (even_offset == 1) {
            /* Right offset */
            output = lead + 1 - box_width / 2;
            trail  = lead - box_width;
        } else if (even_offset == -1) {
            /* Left offset */
            output = lead - box_width / 2;
            trail  = lead - box_width;
        } else {
            /* If even_offset isn't 1 or -1, there's some error. */
            g_assert_not_reached ();
        }
    }

    /* Initialize accumulator */
    for (i = 0; i < bpp; i++)
        ac[i] = 0;

    /* As the kernel moves across the image, it has a leading edge and a
     * trailing edge, and the output is in the middle. */
    while (output < len) {
        /* The number of pixels that are both in the image and
         * currently covered by the kernel. This is necessary to
         * handle edge cases. */
        guint coverage = (lead < len ? lead : len - 1) - (trail >= 0 ? trail : -1);

#ifdef READABLE_BOXBLUR_CODE
/* The code here does the same as the code below, but the code below
 * has been optimized by moving the if statements out of the tight for
 * loop, and is harder to understand.
 * Don't use both this code and the code below. */
        for (i = 0; i < bpp; i++) {
            /* If the leading edge of the kernel is still on the image,
             * add the value there to the accumulator. */
            if (lead < len)
                ac[i] += src[bpp * lead + i];

            /* If the trailing edge of the kernel is on the image,
             * subtract the value there from the accumulator. */
            if (trail >= 0)
                ac[i] -= src[bpp * trail + i];

            /* Take the averaged value in the accumulator and store
             * that value in the output. The number of pixels currently
             * stored in the accumulator can be less than the nominal
             * width of the kernel because the kernel can go "over the edge"
             * of the image. */
            if (output >= 0)
                dest[bpp * output + i] = (ac[i] + (coverage >> 1)) / coverage;
        }
#endif

        /* If the leading edge of the kernel is still on the image... */
        if (lead < len) {
            if (trail >= 0) {
                /* If the trailing edge of the kernel is on the image. (Since
                 * the output is in between the lead and trail, it must be on
                 * the image. */
                for (i = 0; i < bpp; i++) {
                    ac[i] += src[bpp * lead + i];
                    ac[i] -= src[bpp * trail + i];
                    dest[bpp * output + i] = (ac[i] + (coverage >> 1)) / coverage;
                }
            } else if (output >= 0) {
                /* If the output is on the image, but the trailing edge isn't yet
                 * on the image. */

                for (i = 0; i < bpp; i++) {
                    ac[i] += src[bpp * lead + i];
                    dest[bpp * output + i] = (ac[i] + (coverage >> 1)) / coverage;
                }
            } else {
                /* If leading edge is on the image, but the output and trailing
                 * edge aren't yet on the image. */
                for (i = 0; i < bpp; i++)
                    ac[i] += src[bpp * lead + i];
            }
        } else if (trail >= 0) {
            /* If the leading edge has gone off the image, but the output and
             * trailing edge are on the image. (The big loop exits when the
             * output goes off the image. */
            for (i = 0; i < bpp; i++) {
                ac[i] -= src[bpp * trail + i];
                dest[bpp * output + i] = (ac[i] + (coverage >> 1)) / coverage;
            }
        } else if (output >= 0) {
            /* Leading has gone off the image and trailing isn't yet in it
             * (small image) */
            for (i = 0; i < bpp; i++)
                dest[bpp * output + i] = (ac[i] + (coverage >> 1)) / coverage;
        }

        lead++;
        output++;
        trail++;
    }

    g_free (ac);
}