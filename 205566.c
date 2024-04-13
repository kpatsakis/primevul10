make_gaussian_convolution_matrix (gdouble radius, gdouble **out_matrix, gint *out_matrix_len)
{
    gdouble *matrix;
    gdouble std_dev;
    gdouble sum;
    gint matrix_len;
    gint i, j;

    std_dev = radius + 1.0;
    radius = std_dev * 2;

    matrix_len = 2 * ceil (radius - 0.5) + 1;
    if (matrix_len <= 0)
        matrix_len = 1;

    matrix = g_new0 (gdouble, matrix_len);

    /* Fill the matrix by doing numerical integration approximation
     * from -2*std_dev to 2*std_dev, sampling 50 points per pixel.
     * We do the bottom half, mirror it to the top half, then compute the
     * center point.  Otherwise asymmetric quantization errors will occur.
     * The formula to integrate is e^-(x^2/2s^2).
     */

    for (i = matrix_len / 2 + 1; i < matrix_len; i++)
    {
        gdouble base_x = i - (matrix_len / 2) - 0.5;

        sum = 0;
        for (j = 1; j <= 50; j++)
        {
            gdouble r = base_x + 0.02 * j;

            if (r <= radius)
                sum += exp (- SQR (r) / (2 * SQR (std_dev)));
        }

        matrix[i] = sum / 50;
    }

    /* mirror to the bottom half */
    for (i = 0; i <= matrix_len / 2; i++)
        matrix[i] = matrix[matrix_len - 1 - i];

    /* find center val -- calculate an odd number of quanta to make it
     * symmetric, even if the center point is weighted slightly higher
     * than others.
     */
    sum = 0;
    for (j = 0; j <= 50; j++)
        sum += exp (- SQR (- 0.5 + 0.02 * j) / (2 * SQR (std_dev)));

    matrix[matrix_len / 2] = sum / 51;

    /* normalize the distribution by scaling the total sum to one */
    sum = 0;
    for (i = 0; i < matrix_len; i++)
        sum += matrix[i];

    for (i = 0; i < matrix_len; i++)
        matrix[i] = matrix[i] / sum;

    *out_matrix = matrix;
    *out_matrix_len = matrix_len;
}