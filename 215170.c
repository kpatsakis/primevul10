static void convert(float y, float u, float v,
                    float *b, float *g, float *r)
{
    *r = y               + 1.5747f * v;
    *g = y - 0.1873f * u - 0.4682f * v;
    *b = y + 1.8556f * u;
}