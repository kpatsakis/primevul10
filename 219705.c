static void write_i2c_regvals(struct sd *sd,
			const struct ov_i2c_regvals *regvals,
			int n)
{
	while (--n >= 0) {
		i2c_w(sd, regvals->reg, regvals->val);
		regvals++;
	}
}