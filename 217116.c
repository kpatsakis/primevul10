ceilLog2 (int x)
{
    //
    // For x > 0, ceilLog2(y) returns ceil(log(x)/log(2)).
    //

    int y = 0;
    int r = 0;

    while (x > 1)
    {
	if (x & 1)
	    r = 1;

	y +=  1;
	x >>= 1;
    }

    return y + r;
}