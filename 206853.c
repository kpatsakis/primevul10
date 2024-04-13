void round_corners(GVJ_t * job, pointf * AF, int sides, int style, int filled)
{
    pointf *B, C[5], *D, p0, p1;
    double rbconst, d, dx, dy, t;
    int i, seg, mode, shape;
    pointf* pts;

    shape = style & SHAPE_MASK;
    if (style & DIAGONALS)
	mode = DIAGONALS;
    else if (style & SHAPE_MASK)
	mode = shape;
    else
	mode = ROUNDED;
    if (mode == CYLINDER) {
	cylinder_draw (job, AF, sides, style, filled);
	return;
    } 
    B = N_NEW(4 * sides + 4, pointf);
    i = 0;
    /* rbconst is distance offset from a corner of the polygon.
     * It should be the same for every corner, and also never
     * bigger than one-third the length of a side.
     */
    rbconst = RBCONST;
    for (seg = 0; seg < sides; seg++) {
	p0 = AF[seg];
	if (seg < sides - 1)
	    p1 = AF[seg + 1];
	else
	    p1 = AF[0];
	dx = p1.x - p0.x;
	dy = p1.y - p0.y;
	d = sqrt(dx * dx + dy * dy);
	rbconst = MIN(rbconst, d / 3.0);
    }
    for (seg = 0; seg < sides; seg++) {
	p0 = AF[seg];
	if (seg < sides - 1)
	    p1 = AF[seg + 1];
	else
	    p1 = AF[0];
	dx = p1.x - p0.x;
	dy = p1.y - p0.y;
	d = sqrt(dx * dx + dy * dy);
	t = rbconst / d;
	if (shape == BOX3D || shape == COMPONENT)
	    t /= 3;
	else if (shape == DOGEAR)
	    t /= 2;
	if (mode != ROUNDED)
	    B[i++] = p0;
	else 
	    B[i++] = interpolate_pointf(RBCURVE * t, p0, p1);
	B[i++] = interpolate_pointf(t, p0, p1);
	B[i++] = interpolate_pointf(1.0 - t, p0, p1);
	if (mode == ROUNDED)
	    B[i++] = interpolate_pointf(1.0 - RBCURVE * t, p0, p1);
    }
    B[i++] = B[0];
    B[i++] = B[1];
    B[i++] = B[2];

    switch (mode) {
    case ROUNDED:
	pts = N_GNEW(6 * sides + 2, pointf);
	i = 0;
	for (seg = 0; seg < sides; seg++) {
	    pts[i++] = B[4 * seg];
	    pts[i++] = B[4 * seg+1];
	    pts[i++] = B[4 * seg+1];
	    pts[i++] = B[4 * seg+2];
	    pts[i++] = B[4 * seg+2];
	    pts[i++] = B[4 * seg+3];
	}
	pts[i++] = pts[0];
	pts[i++] = pts[1];
	gvrender_beziercurve(job, pts+1, i-1, FALSE, FALSE, filled);
	free (pts);
	
#if 0
	if (filled) {
	    pointf *pts = N_GNEW(2 * sides, pointf);
		pts[j++] = B[4 * seg + 1];
		pts[j++] = B[4 * seg + 2];
	    }
	    gvrender_polygon(job, pts, 2 * sides, filled);
	    free(pts);
	    for (seg = 0; seg < sides; seg++) {
	    }
	}
	if (penc) {
	    for (seg = 0; seg < sides; seg++) {
		gvrender_polyline(job, B + 4 * seg + 1, 2);
		gvrender_beziercurve(job, B + 4 * seg + 2, 4, FALSE, FALSE, FALSE);
	    }
	}
#endif
	break;
    case DIAGONALS:
	/* diagonals are weird.  rewrite someday. */
	gvrender_polygon(job, AF, sides, filled);

	for (seg = 0; seg < sides; seg++) {
#ifdef NOTDEF
	    C[0] = B[3 * seg];
	    C[1] = B[3 * seg + 3];
	    gvrender_polyline(job, C, 2);
#endif
	    C[0] = B[3 * seg + 2];
	    C[1] = B[3 * seg + 4];
	    gvrender_polyline(job, C, 2);
	}
	break;
    case DOGEAR:
	/* Add the cutoff edge. */
	D = N_NEW(sides + 1, pointf);
	for (seg = 1; seg < sides; seg++)
	    D[seg] = AF[seg];
	D[0] = B[3 * (sides - 1) + 4];
	D[sides] = B[3 * (sides - 1) + 2];
	gvrender_polygon(job, D, sides + 1, filled);
	free(D);

	/* Draw the inner edge. */
	seg = sides - 1;
	C[0] = B[3 * seg + 2];
	C[1] = B[3 * seg + 4];
	C[2].x = C[1].x + (C[0].x - B[3 * seg + 3].x);
	C[2].y = C[1].y + (C[0].y - B[3 * seg + 3].y);
	gvrender_polyline(job, C + 1, 2);
	C[1] = C[2];
	gvrender_polyline(job, C, 2);
	break;
    case TAB:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  D[3] +--+ D[2]
	 *       |  |          B[1]
	 *  B[3] +  +----------+--+ AF[0]=B[0]=D[0]
	 *       |  B[2]=D[1]     |
	 *  B[4] +                |
	 *       |                |
	 *  B[5] +                |
	 *       +----------------+
	 *
	 */
	/* Add the tab edges. */
	D = N_NEW(sides + 2, pointf);
	D[0] = AF[0];
	D[1] = B[2];
	D[2].x = B[2].x + (B[3].x - B[4].x) / 3;
	D[2].y = B[2].y + (B[3].y - B[4].y) / 3;
	D[3].x = B[3].x + (B[3].x - B[4].x) / 3;
	D[3].y = B[3].y + (B[3].y - B[4].y) / 3;
	for (seg = 4; seg < sides + 2; seg++)
	    D[seg] = AF[seg - 2];
	gvrender_polygon(job, D, sides + 2, filled);
	free(D);


	/* Draw the inner edge. */
	C[0] = B[3];
	C[1] = B[2];
	gvrender_polyline(job, C, 2);
	break;
    case FOLDER:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *            D[2] +----+ D[1]
	 *  B[3]=         /      \
	 *  D[4] +--+----+     +  + AF[0]=B[0]=D[0]
	 *       |  B[2] D[3] B[1]|
	 *  B[4] +                |
	 *       |                |
	 *  B[5] +                |
	 *       +----------------+
	 *
	 */
	/* Add the folder edges. */
	D = N_NEW(sides + 3, pointf);
	D[0] = AF[0];
	D[1].x = AF[0].x - (AF[0].x - B[1].x) / 4;
	D[1].y = AF[0].y + (B[3].y - B[4].y) / 3;
	D[2].x = AF[0].x - 2 * (AF[0].x - B[1].x);
	D[2].y = D[1].y;
	D[3].x = AF[0].x - 2.25 * (AF[0].x - B[1].x);
	D[3].y = B[3].y;
	D[4].x = B[3].x;
	D[4].y = B[3].y;
	for (seg = 4; seg < sides + 3; seg++)
	    D[seg] = AF[seg - 3];
	gvrender_polygon(job, D, sides + 3, filled);
	free(D);
	break;
    case BOX3D:
	assert(sides == 4);
	/* Adjust for the cutoff edges. */
	D = N_NEW(sides + 2, pointf);
	D[0] = AF[0];
	D[1] = B[2];
	D[2] = B[4];
	D[3] = AF[2];
	D[4] = B[8];
	D[5] = B[10];
	gvrender_polygon(job, D, sides + 2, filled);
	free(D);

	/* Draw the inner vertices. */
	C[0].x = B[1].x + (B[11].x - B[0].x);
	C[0].y = B[1].y + (B[11].y - B[0].y);
	C[1] = B[4];
	gvrender_polyline(job, C, 2);
	C[1] = B[8];
	gvrender_polyline(job, C, 2);
	C[1] = B[0];
	gvrender_polyline(job, C, 2);
	break;
    case COMPONENT:
	assert(sides == 4);
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  D[1] +----------------+ D[0]
	 *       |                |
	 *  3+---+2               |
	 *   |                    |
	 *  4+---+5               |
	 *       |                |
	 *  7+---+6               |
	 *   |                    |
	 *  8+---+9               |
	 *       |                |
	 *     10+----------------+ D[11]
	 *
	 */
	D = N_NEW(sides + 8, pointf);
	D[0] = AF[0];
	D[1] = AF[1];
	D[2].x = B[3].x + (B[4].x - B[3].x);
	D[2].y = B[3].y + (B[4].y - B[3].y);
	D[3].x = D[2].x + (B[3].x - B[2].x);
	D[3].y = D[2].y + (B[3].y - B[2].y);
	D[4].x = D[3].x + (B[4].x - B[3].x);
	D[4].y = D[3].y + (B[4].y - B[3].y);
	D[5].x = D[4].x + (D[2].x - D[3].x);
	D[5].y = D[4].y + (D[2].y - D[3].y);

	D[9].x = B[6].x + (B[5].x - B[6].x);
	D[9].y = B[6].y + (B[5].y - B[6].y);
	D[8].x = D[9].x + (B[6].x - B[7].x);
	D[8].y = D[9].y + (B[6].y - B[7].y);
	D[7].x = D[8].x + (B[5].x - B[6].x);
	D[7].y = D[8].y + (B[5].y - B[6].y);
	D[6].x = D[7].x + (D[9].x - D[8].x);
	D[6].y = D[7].y + (D[9].y - D[8].y);

	D[10] = AF[2];
	D[11] = AF[3];
	gvrender_polygon(job, D, sides + 8, filled);

	/* Draw the internal vertices. */
	C[0] = D[2];
	C[1].x = D[2].x - (D[3].x - D[2].x);
	C[1].y = D[2].y - (D[3].y - D[2].y);
	C[2].x = C[1].x + (D[4].x - D[3].x);
	C[2].y = C[1].y + (D[4].y - D[3].y);
	C[3] = D[5];
	gvrender_polyline(job, C, 4);
	C[0] = D[6];
	C[1].x = D[6].x - (D[7].x - D[6].x);
	C[1].y = D[6].y - (D[7].y - D[6].y);
	C[2].x = C[1].x + (D[8].x - D[7].x);
	C[2].y = C[1].y + (D[8].y - D[7].y);
	C[3] = D[9];
	gvrender_polyline(job, C, 4);

	free(D);
	break;

    case PROMOTER:
	/*
	 * L-shaped arrow on a center line, scales in the x direction
	 *
	 *  
	 *      D[1]	          |\
	 *       +----------------+ \
	 *       |	        D[0] \
	 *       |                    \
	 *       |                    /    
	 *       |             D[5]  /
	 *       |        +-------+ /
	 *       |	  |       |/
	 *	 +--------+
	 */
	/* Add the tab edges. */
				
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//the arrow's thickness is (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides + 5, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (AF[0].x - AF[1].x)/8; //x_center + width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)*3/2; //D[4].y + width
	D[1].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (AF[0].x - AF[1].x)/4; //x_center - 2*width
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	D[3].x = D[2].x + (B[2].x - B[3].x)/2; //D[2].x + width
	D[3].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	D[4].x = D[3].x;
	D[4].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y); //highest cds point 
	D[5].x = D[0].x;
	D[5].y = D[4].y; //highest cds point
	D[6].x = D[0].x;
	D[6].y = D[4].y - (B[3].y-B[4].y)/4; //D[4].y - width/2 
	D[7].x = D[6].x + (B[2].x - B[3].x); //D[6].x + 2*width
	D[7].y = D[6].y + (B[3].y - B[4].y)/2; //D[6].y + width
	D[8].x = D[0].x;
	D[8].y = D[0].y + (B[3].y - B[4].y)/4;//D[0].y + width/2
	gvrender_polygon(job, D, sides + 5, filled);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);			
			
	break;
				
    case CDS:
	/*
	 * arrow without the protrusions, scales normally
	 *
	 *  
	 *      D[1] = AF[1]      
	 *       +----------------+\
	 *       |		D[0]\
	 *       |                   \
	 *       |                   /    
	 *       |                  /
	 *       +----------------+/
	 *	  	          D[3]
	 *	 
	 */
	D = N_NEW(sides + 1, pointf);
	D[0].x = B[1].x;
	D[0].y = B[1].y - (B[3].y - B[4].y)/2;
	D[1].x = B[3].x;
	D[1].y = B[3].y - (B[3].y - B[4].y)/2;
	D[2].x = AF[2].x;
	D[2].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[3].x = B[1].x;
	D[3].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[4].y = AF[0].y - (AF[0].y - AF[3].y)/2;
	D[4].x = AF[0].x;
				
	gvrender_polygon(job, D, sides + 1, filled);
	free(D);

	break;

    case TERMINATOR:
	/*
	* T-shape, does not scale, always in the center
	*
	*  
	*      D[4]      
	*       +----------------+
	*       |		D[3]
	*       |                |
	*       |                |    
	*       |  D[6]    D[1]  |
	*   D[5]+---+       +----+ D[2]
	*	    |	    |     
	*	    +-------+ D[0]
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	D = N_NEW(sides + 4, pointf);
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/4; //x_center + width/2
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/2;
	D[2].x = D[1].x + (B[2].x-B[3].x)/2;
	D[2].y = D[1].y;
	D[3].x = D[2].x;
	D[3].y = D[2].y + (B[3].y-B[4].y)/2;
	D[4].x = AF[1].x + (AF[0].x-AF[1].x)/2 - (B[2].x-B[3].x)*3/4; //D[3].y mirrowed across the center
	D[4].y = D[3].y;
	D[5].x = D[4].x;
	D[5].y = D[2].y;
	D[6].x = AF[1].x + (AF[0].x-AF[1].x)/2 - (B[2].x-B[3].x)/4; //D[1].x mirrowed across the center
	D[6].y = D[1].y;
	D[7].x = D[6].x;
	D[7].y = D[0].y;
	gvrender_polygon(job, D, sides + 4, filled);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;

    case UTR:
	/*
	 * half-octagon with line, does not scale, always in center
	 *
	 *  D[3]
	 *     _____  D[2] 
	 *    /     \
	 *   /       \ D[1]
	 *   |       |
	 *   -----------
	 *              D[0]   
	 *      
	 *	
	 *	          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	D = N_NEW(sides + 2, pointf);
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)*3/4; //x_center+width	
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/4; //D[0].y+width/2
	D[2].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/4; //x_center+width/2
	D[2].y = D[1].y + (B[3].y-B[4].y)/2; //D[1].y+width
	D[3].x = AF[1].x + (AF[0].x-AF[1].x)/2 - (B[2].x-B[3].x)/4; //D[2].x mirrowed across the center
	D[3].y = D[2].y;
	D[4].x = AF[1].x + (AF[0].x-AF[1].x)/2 - (B[2].x-B[3].x)*3/4;
	D[4].y = D[1].y;
	D[5].x = D[4].x;
	D[5].y = D[0].y;
	gvrender_polygon(job, D, sides + 2, filled);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;
    case PRIMERSITE:
	/*
	* half arrow shape, scales in the x-direction
	*                 D[1]
	*		    |\
	*		    | \
	*		    |  \
	*	------------    \
	*	|		 \
	*	------------------\ D[0]			 
	*				
	*   --------------------------------
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides + 1, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x);//x_center + width*2
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/4;//y_center + 1/2 width
	D[1].x = D[0].x - (B[2].x-B[3].x); //x_center
	D[1].y = D[0].y + (B[3].y-B[4].y);
	D[2].x = D[1].x;
	D[2].y = D[0].y + (B[3].y-B[4].y)/2;
	D[3].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (AF[0].x - AF[1].x)/4;//x_center - 2*(scalable width)
	D[3].y = D[2].y;
	D[4].x = D[3].x;
	D[4].y = D[0].y;
	gvrender_polygon(job, D, sides + 1, filled);
				
	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;
    case RESTRICTIONSITE:
	/*
	* zigzag shape, scales in the x-direction (only the middle section)
	*
	*		 
	*   ----D[2]	 
	*   |   |________ D[0]
	*   |            |____
	*   ----------	 |
	*   D[4]      --- D[7]
	*				
	*   
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides + 4, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (AF[0].x - AF[1].x)/8 + (B[2].x-B[3].x)/2;//x_center + scalable_width + width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/4;//y_center + 1/2 width
	D[1].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (AF[0].x - AF[1].x)/8; //x_center - width
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[2].x - (B[2].x-B[3].x)/2; //D[2].x - width
	D[3].y = D[2].y;
	D[4].x = D[3].x;
	D[4].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)/4; //y_center - 1/2(width)
	D[5].x = D[0].x - (B[2].x-B[3].x)/2;
	D[5].y = D[4].y;
	D[6].x = D[5].x;
	D[6].y = D[5].y - (B[3].y-B[4].y)/2;
	D[7].x = D[0].x;
	D[7].y = D[6].y;
	gvrender_polygon(job, D, sides + 4, filled);
				
	/*dsDNA line left half*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = D[4].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);

	/*dsDNA line right half*/
	C[0].x = D[7].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);			    
	free(D);

	break;	
    case FIVEPOVERHANG:
	/*
	*  does not scale, on the left side
	*
	*  D[3]------D[2]	 
	*  |          |
	*  D[0]------D[1]
	*        -----  ------------
	*        |    |
	*       D[0]--D[1]
	*				
	*   
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x;//the very left edge
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8;//y_center + 1/4 width
	D[1].x = D[0].x + 2*(B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*second, lower shape*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (B[2].x-B[3].x);
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)*5/8; //y_center - 5/4 width
	D[1].x = D[0].x + (B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*dsDNA line right half*/
	C[0].x = D[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);			    
	free(D);

	break;	
    case THREEPOVERHANG:
	/*
	*  does not scale, on the right side
	*
	*	   D[2]------D[1]	 
	*	   |          |
	*----------D[3]------D[0]
	*	   -----  D[1]
	*          |    |
	*          D[3]--D[0]
	*				
	*   
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides, pointf);
	D[0].x = AF[0].x;//the very right edge
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8;//y_center + 1/4 width
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/2;
	D[2].x = D[1].x - 2*(B[3].y-B[4].y);
	D[2].y = D[1].y;
	D[3].x = D[2].x;
	D[3].y = D[0].y;
	gvrender_polygon(job, D, sides, filled);

	/*second, lower shape*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[0].x - (B[2].x-B[3].x);
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)*5/8; //y_center - 5/4 width
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/2;
	D[2].x = D[1].x - (B[3].y-B[4].y);
	D[2].y = D[1].y;
	D[3].x = D[2].x;
	D[3].y = D[0].y;
	gvrender_polygon(job, D, sides, filled);

	/*dsDNA line left half*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = D[3].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);			    
	free(D);

	break;	
    case NOVERHANG:
	/*
	*  does not scale
	*
	*     D[3]------D[2]   D[3]------D[2]    
	*     |          |      |          |
	*  ---D[0]------D[1]   D[0]------D[1]----
	*     D[3]------D[2]   D[3]------D[2]    
	*     |          |	|          |
	*     D[0]------D[1]   D[0]------D[1]
	*        
	*				
	*   
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	/*upper left rectangle*/
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)*9/8; //x_center - 2*width - 1/4*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8;//y_center + 1/4 width
	D[1].x = D[0].x + (B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*lower, left rectangle*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)*9/8; //x_center - 2*width - 1/4*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)*5/8;//y_center - width - 1/4 width
	D[1].x = D[0].x + (B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*lower, right rectangle*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x)/8; //x_center + 1/4*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)*5/8;//y_center - width - 1/4 width
	D[1].x = D[0].x + (B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*upper, right rectangle*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x)/8; //x_center + 1/4*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8;//y_center - width - 1/4 width
	D[1].x = D[0].x + (B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);
	
	/*dsDNA line right half*/
	C[0].x = D[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);

	/*dsDNA line left half*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)*9/8; //D[0].x of of the left rectangles
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[1].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);			    
	free(D);

	break;	
    case ASSEMBLY:
	/*
	*  does not scale
	*
	*      D[3]----------D[2]	 
	*      |               |
	*     D[0]----------D[1]
	* ----                  ---------
	*      D[3]----------D[2]	 
	*      |               |
	*     D[0]----------D[1]
	*  
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x); //x_center - 2*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8;//y_center + 1/4 width
	D[1].x = D[0].x + 2*(B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*second, lower shape*/
	free(D);
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x); //x_center - 2*width
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)*5/8;//y_center - width - 1/4 width
	D[1].x = D[0].x + 2*(B[2].x-B[3].x);
	D[1].y = D[0].y;
	D[2].x = D[1].x;
	D[2].y = D[1].y + (B[3].y-B[4].y)/2;
	D[3].x = D[0].x;
	D[3].y = D[2].y;
	gvrender_polygon(job, D, sides, filled);

	/*dsDNA line right half*/
	C[0].x = D[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);

	/*dsDNA line left half*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = D[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);
	free(D);

	break;	
    case SIGNATURE:
	/*
	*   
	* 
	*   +--------------+
	*   |		   |
	*   |x		   |
	*   |_____________ |
	*   +--------------+
	*/
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2;
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	//the thickness is subituted with (AF[0].x - AF[1].x)/8 to make it scalable in the y with label length
	D = N_NEW(sides, pointf);
	D[0].x = AF[0].x;
	D[0].y = B[1].y - (B[3].y - B[4].y)/2;
	D[1].x = B[3].x;
	D[1].y = B[3].y - (B[3].y - B[4].y)/2;
	D[2].x = AF[2].x;
	D[2].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[3].x = AF[0].x;
	D[3].y = AF[2].y + (B[3].y - B[4].y)/2;
	gvrender_polygon(job, D, sides, filled);

	/* "\" of the X*/
	C[0].x = AF[1].x + (B[2].x-B[3].x)/4;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/8; //y_center + 1/4 width
	C[1].x = C[0].x + (B[2].x-B[3].x)/4;//C[0].x + width/2
	C[1].y = C[0].y - (B[3].y-B[4].y)/4;//C[0].y - width/2
	gvrender_polyline(job, C, 2);
	
	/*"/" of the X*/
	C[0].x = AF[1].x + (B[2].x-B[3].x)/4;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[3].y-B[4].y)/8; //y_center - 1/4 width
	C[1].x = C[0].x + (B[2].x-B[3].x)/4;//C[0].x + width/2
	C[1].y = C[0].y + (B[3].y-B[4].y)/4;//C[0].y + width/2
	gvrender_polyline(job, C, 2);	

	/*bottom line*/
	C[0].x = AF[1].x + (B[2].x-B[3].x)/4;
	C[0].y = AF[2].y + (B[3].y-B[4].y)*3/4;
	C[1].x = AF[0].x - (B[2].x-B[3].x)/4;
	C[1].y = C[0].y;
	gvrender_polyline(job, C, 2);
	free(D);

	break;	
    case INSULATOR:
	/*
	 * double square
	 *
	 *  +-----+
	 *--| ___ |---
	 *  | |_| |
	 *  +-----+
	 *	          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	D = N_NEW(sides, pointf);
	D[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x)/2; //x_center+width	
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[2].x-B[3].x)/2; //y_center
	D[1].x = D[0].x;
	D[1].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[2].x-B[3].x)/2; //D[0].y- width
	D[2].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)/2; //x_center-width
	D[2].y = D[1].y;
	D[3].x = D[2].x;
	D[3].y = D[0].y;
	gvrender_polygon(job, D, sides, filled);
	free(D);

	/*outer square line*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x)*3/4; //x_center+1.5*width	
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[2].x-B[3].x)*3/4; //y_center
	C[1].x = C[0].x;
	C[1].y = AF[2].y + (AF[1].y - AF[2].y)/2 - (B[2].x-B[3].x)*3/4; //y_center- 1.5*width
	C[2].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)*3/4; //x_center-1.5*width
	C[2].y = C[1].y;
	C[3].x = C[2].x;
	C[3].y = C[0].y;
	C[4] = C[0];
	gvrender_polyline(job, C, 5);		        
	
	/*dsDNA line right half*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2 + (B[2].x-B[3].x)*3/4;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);

	/*dsDNA line left half*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[1].x + (AF[0].x - AF[1].x)/2 - (B[2].x-B[3].x)*3/4;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);

	break;
    case RIBOSITE:
	/*
	 * X with a dashed line on the bottom
	 * 
	 *
	 *           X
	 *	     |
	 *	------------          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;

	D = N_NEW(sides + 12, pointf); //12-sided x
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/4; //x_center+widtht/2 , lower right corner of the x
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/2; //y_center + width
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/8; //D[0].y +width/4
	D[2].x = D[0].x - (B[2].x-B[3].x)/8; //D[0].x- width/4 //right nook of the x
	D[2].y = D[1].y + (B[3].y-B[4].y)/8; //D[0].y+width/2 or D[1].y+width/4
	D[3].x = D[0].x;
	D[3].y = D[2].y + (B[3].y-B[4].y)/8; //D[2].y + width/4
	D[4].x = D[0].x;
	D[4].y = D[3].y + (B[3].y-B[4].y)/8; //top right corner of the x
	D[5].x = D[2].x;
	D[5].y = D[4].y;
	D[6].x = AF[1].x + (AF[0].x - AF[1].x)/2; //x_center
	D[6].y = D[3].y; //top nook
	D[7].x = D[6].x - (B[2].x-B[3].x)/8; //D[5] mirrowed across y
	D[7].y = D[5].y;
	D[8].x = D[7].x - (B[2].x-B[3].x)/8;//top left corner
	D[8].y = D[7].y;
	D[9].x = D[8].x;
	D[9].y = D[3].y;
	D[10].x = D[8].x + (B[2].x-B[3].x)/8;
	D[10].y = D[2].y;
	D[11].x = D[8].x;
	D[11].y = D[1].y;
	D[12].x = D[8].x;
	D[12].y = D[0].y;
	D[13].x = D[10].x;
	D[13].y = D[12].y;
	D[14].x = D[6].x; //bottom nook
	D[14].y = D[1].y;
	D[15].x = D[2].x;
	D[15].y = D[0].y;
	gvrender_polygon(job, D, sides + 12, filled);

	//2-part dash line

	/*line below the x, bottom dash*/
	C[0].x = D[14].x; //x_center
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	C[1].x = C[0].x;
	C[1].y = C[0].y + (B[3].y-B[4].y)/8; //y_center + 1/4*width
	gvrender_polyline(job, C, 2);

	/*line below the x, top dash*/
	C[0].x = D[14].x; //x_center
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/4;
	C[1].x = C[0].x;
	C[1].y = C[0].y + (B[3].y-B[4].y)/8;
	gvrender_polyline(job, C, 2);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;
    case RNASTAB:
	/*
	 * hexagon with a dashed line on the bottom
	 * 
	 *
	 *           O
	 *	     |
	 *	------------          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;

	D = N_NEW(sides + 4, pointf); //12-sided x
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/8; //x_center+widtht/8 , lower right corner of the hexagon
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/2; //y_center + width
	D[1].x = D[0].x + (B[2].x-B[3].x)/8;
	D[1].y = D[0].y + (B[3].y-B[4].y)/8; //D[0].y +width/4
	D[2].x = D[1].x; //D[0].x- width/4
	D[2].y = D[1].y + (B[3].y-B[4].y)/4; //D[1].y+width/2
	D[3].x = D[0].x;
	D[3].y = D[2].y + (B[3].y-B[4].y)/8; //D[2].y + width/4
	D[4].x = D[3].x - (B[2].x-B[3].x)/4;
	D[4].y = D[3].y; //top of the hexagon
	D[5].x = D[4].x - (B[2].x-B[3].x)/8;
	D[5].y = D[2].y;
	D[6].x = D[5].x;
	D[6].y = D[1].y; //left side
	D[7].x = D[4].x;
	D[7].y = D[0].y; //bottom
	gvrender_polygon(job, D, sides + 4, filled);

	//2-part dash line

	/*line below the x, bottom dash*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2; //x_center
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	C[1].x = C[0].x;
	C[1].y = C[0].y + (B[3].y-B[4].y)/8; //y_center + 1/4*width
	gvrender_polyline(job, C, 2);

	/*line below the x, top dash*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2; //x_center
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/4;
	C[1].x = C[0].x;
	C[1].y = C[0].y + (B[3].y-B[4].y)/8;
	gvrender_polyline(job, C, 2);



	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;
    case PROTEASESITE:
	/*
	 * X with a solid line on the bottom
	 * 
	 *
	 *           X
	 *	     |
	 *	------------          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;
	D = N_NEW(sides + 12, pointf); //12-sided x
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/4; //x_center+widtht/2 , lower right corner of the x
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/2; //y_center + width
	D[1].x = D[0].x;
	D[1].y = D[0].y + (B[3].y-B[4].y)/8; //D[0].y +width/4
	D[2].x = D[0].x - (B[2].x-B[3].x)/8; //D[0].x- width/4 //right nook of the x
	D[2].y = D[1].y + (B[3].y-B[4].y)/8; //D[0].y+width/2 or D[1].y+width/4
	D[3].x = D[0].x;
	D[3].y = D[2].y + (B[3].y-B[4].y)/8; //D[2].y + width/4
	D[4].x = D[0].x;
	D[4].y = D[3].y + (B[3].y-B[4].y)/8; //top right corner of the x
	D[5].x = D[2].x;
	D[5].y = D[4].y;
	D[6].x = AF[1].x + (AF[0].x - AF[1].x)/2; //x_center
	D[6].y = D[3].y; //top nook
	D[7].x = D[6].x - (B[2].x-B[3].x)/8; //D[5] mirrowed across y
	D[7].y = D[5].y;
	D[8].x = D[7].x - (B[2].x-B[3].x)/8;//top left corner
	D[8].y = D[7].y;
	D[9].x = D[8].x;
	D[9].y = D[3].y;
	D[10].x = D[8].x + (B[2].x-B[3].x)/8;
	D[10].y = D[2].y;
	D[11].x = D[8].x;
	D[11].y = D[1].y;
	D[12].x = D[8].x;
	D[12].y = D[0].y;
	D[13].x = D[10].x;
	D[13].y = D[12].y;
	D[14].x = D[6].x; //bottom nook
	D[14].y = D[1].y;
	D[15].x = D[2].x;
	D[15].y = D[0].y;
	gvrender_polygon(job, D, sides + 12, filled);


	/*line below the x*/
	C[0] = D[14];
	C[1].x = C[0].x;
	C[1].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	gvrender_polyline(job, C, 2);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;
    case PROTEINSTAB:
	/*
	 * hexagon with a dashed line on the bottom
	 * 
	 *
	 *           O
	 *	     |
	 *	------------          
	 */
	//x_center is AF[1].x + (AF[0].x - AF[1].x)/2
	//y_center is AF[2].y + (AF[1].y - AF[2].y)/2;
	//width units are (B[2].x-B[3].x)/2 or (B[3].y-B[4].y)/2;

	D = N_NEW(sides + 4, pointf); //12-sided x
	D[0].x = AF[1].x + (AF[0].x-AF[1].x)/2 + (B[2].x-B[3].x)/8; //x_center+widtht/8 , lower right corner of the hexagon
	D[0].y = AF[2].y + (AF[1].y - AF[2].y)/2 + (B[3].y-B[4].y)/2; //y_center + width
	D[1].x = D[0].x + (B[2].x-B[3].x)/8;
	D[1].y = D[0].y + (B[3].y-B[4].y)/8; //D[0].y +width/4
	D[2].x = D[1].x; //D[0].x- width/4
	D[2].y = D[1].y + (B[3].y-B[4].y)/4; //D[1].y+width/2
	D[3].x = D[0].x;
	D[3].y = D[2].y + (B[3].y-B[4].y)/8; //D[2].y + width/4
	D[4].x = D[3].x - (B[2].x-B[3].x)/4;
	D[4].y = D[3].y; //top of the hexagon
	D[5].x = D[4].x - (B[2].x-B[3].x)/8;
	D[5].y = D[2].y;
	D[6].x = D[5].x;
	D[6].y = D[1].y; //left side
	D[7].x = D[4].x;
	D[7].y = D[0].y; //bottom
	gvrender_polygon(job, D, sides + 4, filled);

	/*line below the x*/
	C[0].x = AF[1].x + (AF[0].x - AF[1].x)/2;
	C[0].y = D[0].y;
	C[1].x = C[0].x;
	C[1].y = AF[2].y + (AF[1].y - AF[2].y)/2; //y_center
	gvrender_polyline(job, C, 2);

	/*dsDNA line*/
	C[0].x = AF[1].x;
	C[0].y = AF[2].y + (AF[1].y - AF[2].y)/2;
	C[1].x = AF[0].x;
	C[1].y = AF[2].y + (AF[0].y - AF[3].y)/2;
	gvrender_polyline(job, C, 2);				
	free(D);

	break;        

    case RPROMOTER:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  
	 *      D[1] = AF[1]      |\
	 *       +----------------+ \
	 *       |	        D[0] \
	 *       |                    \
	 *       |                    /    
	 *       |                   /
	 *       |        +-------+ /
	 *       |	  |       |/
	 *	 +--------+
	 */
	/* Add the tab edges. */
	D = N_NEW(sides + 5, pointf); /*5 new points*/
	D[0].x = B[1].x - (B[2].x - B[3].x)/2;
	D[0].y = B[1].y - (B[3].y - B[4].y)/2;
	D[1].x = B[3].x;
	D[1].y = B[3].y - (B[3].y - B[4].y)/2;
	D[2].x = AF[2].x;
	D[2].y = AF[2].y;
	D[3].x = B[2].x + (B[2].x - B[3].x)/2;
	D[3].y = AF[2].y;
	D[4].x = B[2].x + (B[2].x - B[3].x)/2;
	D[4].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[5].x = B[1].x - (B[2].x - B[3].x)/2;
	D[5].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[6].x = B[1].x - (B[2].x - B[3].x)/2;
	D[6].y = AF[3].y;
	D[7].y = AF[0].y - (AF[0].y - AF[3].y)/2; /*triangle point */
	D[7].x = AF[0].x; /*triangle point */
	D[8].y = AF[0].y;
	D[8].x = B[1].x - (B[2].x - B[3].x)/2;

	gvrender_polygon(job, D, sides + 5, filled);
	free(D);
	break;
				
    case RARROW:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  
	 *      D[1] = AF[1]      |\
	 *       +----------------+ \
	 *       |		D[0] \
	 *       |                    \
	 *       |                    /    
	 *       |                   /
	 *       +----------------+ /
	 *	  	          |/
	 *	 
	 */
	/* Add the tab edges. */
	D = N_NEW(sides + 3, pointf); /*3 new points*/
	D[0].x = B[1].x - (B[2].x - B[3].x)/2;
	D[0].y = B[1].y - (B[3].y - B[4].y)/2;
	D[1].x = B[3].x;
	D[1].y = B[3].y - (B[3].y - B[4].y)/2;
	D[2].x = AF[2].x;
	D[2].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[3].x = B[1].x - (B[2].x - B[3].x)/2;
	D[3].y = AF[2].y + (B[3].y - B[4].y)/2;
	D[4].x = B[1].x - (B[2].x - B[3].x)/2;
	D[4].y = AF[3].y;
	D[5].y = AF[0].y - (AF[0].y - AF[3].y)/2;/*triangle point*/
	D[5].x = AF[0].x; /*triangle point */
	D[6].y = AF[0].y;
	D[6].x = B[1].x - (B[2].x - B[3].x)/2;

	gvrender_polygon(job, D, sides + 3, filled);
	free(D);
	break;

    case LARROW:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  
	 *      /|     
	 *     / +----------------+ 
	 *    /                   |        
	 *    \                   |   
	 *     \ +----------------+ 
	 *	\| 	          
	 *	 
	 */
	/* Add the tab edges. */
	D = N_NEW(sides + 3, pointf); /*3 new points*/
	D[0].x = AF[0].x;
	D[0].y = AF[0].y - (B[3].y-B[4].y)/2;
	D[1].x = B[2].x + (B[2].x - B[3].x)/2;
	D[1].y = AF[0].y - (B[3].y-B[4].y)/2;/*D[0].y*/
	D[2].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[2].y = B[2].y;
	D[3].x = AF[1].x; /*triangle point*/
	D[3].y = AF[1].y - (AF[1].y - AF[2].y)/2; /*triangle point*/
	D[4].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[4].y = AF[2].y;
	D[5].y = AF[2].y + (B[3].y-B[4].y)/2;
	D[5].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[6].y = AF[3].y + (B[3].y - B[4].y)/2;
	D[6].x = AF[0].x;/*D[0]*/

	gvrender_polygon(job, D, sides + 3, filled);
	free(D);
	break;

    case LPROMOTER:
	/*
	 * Adjust the perimeter for the protrusions.
	 *
	 *  
	 *      /|     
	 *     / +----------------+ 
	 *    /   		D[0] 
	 *   /                    |    
	 *   \                    |        
	 *    \                   |   
	 *     \ +--------+       + 
	 *	\| 	  |       |
	 *	          +-------+
	 */
	/* Add the tab edges. */
	D = N_NEW(sides + 5, pointf); /*3 new points*/
	D[0].x = AF[0].x;
	D[0].y = AF[0].y - (B[3].y-B[4].y)/2;
	D[1].x = B[2].x + (B[2].x - B[3].x)/2;
	D[1].y = AF[0].y - (B[3].y-B[4].y)/2;/*D[0].y*/
	D[2].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[2].y = B[2].y;
	D[3].x = AF[1].x; /*triangle point*/
	D[3].y = AF[1].y - (AF[1].y - AF[2].y)/2; /*triangle point*/
	D[4].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[4].y = AF[2].y;
	D[5].y = AF[2].y + (B[3].y-B[4].y)/2;
	D[5].x = B[2].x + (B[2].x - B[3].x)/2;/*D[1].x*/
	D[6].y = AF[3].y + (B[3].y - B[4].y)/2;
	D[6].x = B[1].x - (B[2].x - B[3].x)/2;
	D[7].x = B[1].x - (B[2].x - B[3].x)/2;/*D[6].x*/
	D[7].y = AF[3].y;
	D[8].x = AF[3].x;
	D[8].y = AF[3].y;
				
	gvrender_polygon(job, D, sides + 5, filled);
	free(D);
	break;
    }