static void write_regvals(struct sd *sd,
			 const struct ov_regvals *regvals,
			 int n)
{
	while (--n >= 0) {
		reg_w(sd, regvals->reg, regvals->val);
		regvals++;
	}
}