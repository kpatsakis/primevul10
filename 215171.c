static float to_linear(float x, float scale)
{
    float ax = fabsf(x);

    if (ax <= 1.f) {
        return FFSIGN(x) * powf(ax, 2.2f * scale);
    } else {
        const float log_base = expf(2.2f * scale);

        return FFSIGN(x) * powf(log_base, ax - 1.f);
    }
}