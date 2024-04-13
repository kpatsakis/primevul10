void _udf_err(struct super_block *sb, const char *function,
	      const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	pr_err("error (device %s): %s: %pV", sb->s_id, function, &vaf);

	va_end(args);
}