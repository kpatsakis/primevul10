static void idct_1d(float *blk, int step)
{
    const float a = .5f * cosf(    M_PI / 4.f);
    const float b = .5f * cosf(    M_PI / 16.f);
    const float c = .5f * cosf(    M_PI / 8.f);
    const float d = .5f * cosf(3.f*M_PI / 16.f);
    const float e = .5f * cosf(5.f*M_PI / 16.f);
    const float f = .5f * cosf(3.f*M_PI / 8.f);
    const float g = .5f * cosf(7.f*M_PI / 16.f);

    float alpha[4], beta[4], theta[4], gamma[4];

    alpha[0] = c * blk[2 * step];
    alpha[1] = f * blk[2 * step];
    alpha[2] = c * blk[6 * step];
    alpha[3] = f * blk[6 * step];

    beta[0] = b * blk[1 * step] + d * blk[3 * step] + e * blk[5 * step] + g * blk[7 * step];
    beta[1] = d * blk[1 * step] - g * blk[3 * step] - b * blk[5 * step] - e * blk[7 * step];
    beta[2] = e * blk[1 * step] - b * blk[3 * step] + g * blk[5 * step] + d * blk[7 * step];
    beta[3] = g * blk[1 * step] - e * blk[3 * step] + d * blk[5 * step] - b * blk[7 * step];

    theta[0] = a * (blk[0 * step] + blk[4 * step]);
    theta[3] = a * (blk[0 * step] - blk[4 * step]);

    theta[1] = alpha[0] + alpha[3];
    theta[2] = alpha[1] - alpha[2];

    gamma[0] = theta[0] + theta[1];
    gamma[1] = theta[3] + theta[2];
    gamma[2] = theta[3] - theta[2];
    gamma[3] = theta[0] - theta[1];

    blk[0 * step] = gamma[0] + beta[0];
    blk[1 * step] = gamma[1] + beta[1];
    blk[2 * step] = gamma[2] + beta[2];
    blk[3 * step] = gamma[3] + beta[3];

    blk[4 * step] = gamma[3] - beta[3];
    blk[5 * step] = gamma[2] - beta[2];
    blk[6 * step] = gamma[1] - beta[1];
    blk[7 * step] = gamma[0] - beta[0];
}