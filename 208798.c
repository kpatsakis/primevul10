bool CjfifDecode::PrintValGPS(unsigned nCount, float fCoord1, float fCoord2, float fCoord3,CString &strCoord)
{
	float		fTemp;
	unsigned	nCoordDeg;
	unsigned	nCoordMin;
	float		fCoordSec;

	// TODO: Extend to support 1 & 2 coordinate GPS entries
	if (nCount == 3) {
		nCoordDeg = unsigned(fCoord1);
		nCoordMin = unsigned(fCoord2);
		if (fCoord3 == 0) {
			fTemp = fCoord2 - (float)nCoordMin;
			fCoordSec = fTemp * (float)60.0;
		} else {
			fCoordSec = fCoord3;
		}

		strCoord.Format(_T("%u deg %u' %.3f\""),nCoordDeg,nCoordMin,fCoordSec);
		return true;
	} else {
		strCoord.Format(_T("ERROR: Can't handle %u-comonent GPS coords"),nCount);
		return false;
	}

}