static void PrintFixed(Fixed val, Bool add_space)
{
	if (add_space) fprintf(stderr, " ");
	if (val==FIX_MIN) fprintf(stderr, "-I");
	else if (val==FIX_MAX) fprintf(stderr, "+I");
	else fprintf(stderr, "%g", FIX2FLT(val));
}