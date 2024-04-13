compassPort(node_t * n, boxf * bp, port * pp, char *compass, int sides,
	    inside_t * ictxt)
{
    boxf b;
    pointf p, ctr;
    int rv = 0;
    double theta = 0.0;
    boolean constrain = FALSE;
    boolean dyna = FALSE;
    int side = 0;
    boolean clip = TRUE;
    boolean defined;
    double maxv;  /* sufficiently large value outside of range of node */

    if (bp) {
	b = *bp;
	p = pointfof((b.LL.x + b.UR.x) / 2, (b.LL.y + b.UR.y) / 2);
	defined = TRUE;
    } else {
	p.x = p.y = 0.;
	if (GD_flip(agraphof(n))) {
	    b.UR.x = ND_ht(n) / 2.;
	    b.LL.x = -b.UR.x;
	    b.UR.y = ND_lw(n);
	    b.LL.y = -b.UR.y;
	} else {
	    b.UR.y = ND_ht(n) / 2.;
	    b.LL.y = -b.UR.y;
	    b.UR.x = ND_lw(n);
	    b.LL.x = -b.UR.x;
	}
	defined = FALSE;
    }
    maxv = MAX(b.UR.x,b.UR.y);
    maxv *= 4.0;
    ctr = p;
    if (compass && *compass) {
	switch (*compass++) {
	case 'e':
	    if (*compass)
		rv = 1;
	    else {
                if (ictxt)
                    p = compassPoint(ictxt, ctr.y, maxv);
                else
		    p.x = b.UR.x;
		theta = 0.0;
		constrain = TRUE;
		defined = TRUE;
		clip = FALSE;
		side = sides & RIGHT;
	    }
	    break;
	case 's':
	    p.y = b.LL.y;
	    constrain = TRUE;
	    clip = FALSE;
	    switch (*compass) {
	    case '\0':
		theta = -M_PI * 0.5;
		defined = TRUE;
                if (ictxt)
                    p = compassPoint(ictxt, -maxv, ctr.x);
                else
                    p.x = ctr.x;
		side = sides & BOTTOM;
		break;
	    case 'e':
		theta = -M_PI * 0.25;
		defined = TRUE;
		if (ictxt)
		    p = compassPoint(ictxt, -maxv, maxv);
		else
		    p.x = b.UR.x;
		side = sides & (BOTTOM | RIGHT);
		break;
	    case 'w':
		theta = -M_PI * 0.75;
		defined = TRUE;
		if (ictxt)
		    p = compassPoint(ictxt, -maxv, -maxv);
		else
		    p.x = b.LL.x;
		side = sides & (BOTTOM | LEFT);
		break;
	    default:
		p.y = ctr.y;
		constrain = FALSE;
		clip = TRUE;
		rv = 1;
		break;
	    }
	    break;
	case 'w':
	    if (*compass)
		rv = 1;
	    else {
                if (ictxt)
                    p = compassPoint(ictxt, ctr.y, -maxv);
                else
		    p.x = b.LL.x;
		theta = M_PI;
		constrain = TRUE;
		defined = TRUE;
		clip = FALSE;
		side = sides & LEFT;
	    }
	    break;
	case 'n':
	    p.y = b.UR.y;
	    constrain = TRUE;
	    clip = FALSE;
	    switch (*compass) {
	    case '\0':
		defined = TRUE;
		theta = M_PI * 0.5;
                if (ictxt)
                    p = compassPoint(ictxt, maxv, ctr.x);
                else
                    p.x = ctr.x;
		side = sides & TOP;
		break;
	    case 'e':
		defined = TRUE;
		theta = M_PI * 0.25;
		if (ictxt)
		    p = compassPoint(ictxt, maxv, maxv);
		else
		    p.x = b.UR.x;
		side = sides & (TOP | RIGHT);
		break;
	    case 'w':
		defined = TRUE;
		theta = M_PI * 0.75;
		if (ictxt)
		    p = compassPoint(ictxt, maxv, -maxv);
		else
		    p.x = b.LL.x;
		side = sides & (TOP | LEFT);
		break;
	    default:
		p.y = ctr.y;
		constrain = FALSE;
		clip = TRUE;
		rv = 1;
		break;
	    }
	    break;
	case '_':
	    dyna = TRUE;
	    side = sides;
	    break;
	case 'c':
	    break;
	default:
	    rv = 1;
	    break;
	}
    }
    p = cwrotatepf(p, 90 * GD_rankdir(agraphof(n)));
    if (dyna)
	pp->side = side;
    else
	pp->side = invflip_side(side, GD_rankdir(agraphof(n)));
    pp->bp = bp;
    PF2P(p, pp->p);
    pp->theta = invflip_angle(theta, GD_rankdir(agraphof(n)));
    if ((p.x == 0) && (p.y == 0))
	pp->order = MC_SCALE / 2;
    else {
	/* compute angle with 0 at north pole, increasing CCW */
	double angle = atan2(p.y, p.x) + 1.5 * M_PI;
	if (angle >= 2 * M_PI)
	    angle -= 2 * M_PI;
	pp->order = (int) ((MC_SCALE * angle) / (2 * M_PI));
    }
    pp->constrained = constrain;
    pp->defined = defined;
    pp->clip = clip;
    pp->dyna = dyna;
    return rv;
}