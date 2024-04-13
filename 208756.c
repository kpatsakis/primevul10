CStr2 CjfifDecode::LookupMakerCanonTag(unsigned nMainTag,unsigned nSubTag,unsigned nVal)
{
	CString		strTmp;
	CStr2		sRetVal;

	sRetVal.strTag = _T("???");
	sRetVal.bUnknown = false;				// Set to true in default clauses
	sRetVal.strVal.Format(_T("%u"),nVal);	// Provide default value

	unsigned nValHi,nValLo;
	nValHi = (nVal & 0xff00) >> 8;
	nValLo = (nVal & 0x00ff);

	switch(nMainTag)
	{

	case 0x0001:

		switch(nSubTag)
		{
		case 0x0001: sRetVal.strTag = _T("Canon.Cs1.Macro");break; // Short Macro mode 
		case 0x0002: sRetVal.strTag = _T("Canon.Cs1.Selftimer");break; // Short Self timer 
		case 0x0003: sRetVal.strTag = _T("Canon.Cs1.Quality");
			if (nVal == 2) { sRetVal.strVal = _T("norm"); }
			else if (nVal == 3) { sRetVal.strVal = _T("fine"); }
			else if (nVal == 5) { sRetVal.strVal = _T("superfine"); }
			else {
				sRetVal.strVal = _T("?");
			}
			// Save the quality string for later
			m_strImgQualExif = sRetVal.strVal;
			break; // Short Quality 
		case 0x0004: sRetVal.strTag = _T("Canon.Cs1.FlashMode");break; // Short Flash mode setting 
		case 0x0005: sRetVal.strTag = _T("Canon.Cs1.DriveMode");break; // Short Drive mode setting 
		case 0x0007: sRetVal.strTag = _T("Canon.Cs1.FocusMode"); // Short Focus mode setting 
			switch(nVal) {
				case 0 : sRetVal.strVal = _T("One-shot");break;
				case 1 : sRetVal.strVal = _T("AI Servo");break;
				case 2 : sRetVal.strVal = _T("AI Focus");break;
				case 3 : sRetVal.strVal = _T("Manual Focus");break;
				case 4 : sRetVal.strVal = _T("Single");break;
				case 5 : sRetVal.strVal = _T("Continuous");break;
				case 6 : sRetVal.strVal = _T("Manual Focus");break;
				default : sRetVal.strVal = _T("?");break;
			}
			break;
		case 0x000a: sRetVal.strTag = _T("Canon.Cs1.ImageSize"); // Short Image size 
			if (nVal == 0) { sRetVal.strVal = _T("Large"); }
			else if (nVal == 1) { sRetVal.strVal = _T("Medium"); }
			else if (nVal == 2) { sRetVal.strVal = _T("Small"); }
			else {
				sRetVal.strVal = _T("?");
			}
			break;
		case 0x000b: sRetVal.strTag = _T("Canon.Cs1.EasyMode");break; // Short Easy shooting mode 
		case 0x000c: sRetVal.strTag = _T("Canon.Cs1.DigitalZoom");break; // Short Digital zoom 
		case 0x000d: sRetVal.strTag = _T("Canon.Cs1.Contrast");break; // Short Contrast setting 
		case 0x000e: sRetVal.strTag = _T("Canon.Cs1.Saturation");break; // Short Saturation setting 
		case 0x000f: sRetVal.strTag = _T("Canon.Cs1.Sharpness");break; // Short Sharpness setting 
		case 0x0010: sRetVal.strTag = _T("Canon.Cs1.ISOSpeed");break; // Short ISO speed setting 
		case 0x0011: sRetVal.strTag = _T("Canon.Cs1.MeteringMode");break; // Short Metering mode setting 
		case 0x0012: sRetVal.strTag = _T("Canon.Cs1.FocusType");break; // Short Focus type setting 
		case 0x0013: sRetVal.strTag = _T("Canon.Cs1.AFPoint");break; // Short AF point selected 
		case 0x0014: sRetVal.strTag = _T("Canon.Cs1.ExposureProgram");break; // Short Exposure mode setting 
		case 0x0016: sRetVal.strTag = _T("Canon.Cs1.LensType");break; // 
		case 0x0017: sRetVal.strTag = _T("Canon.Cs1.Lens");break; // Short 'long' and 'short' focal length of lens (in 'focal m_nImgUnits') and 'focal m_nImgUnits' per mm 
		case 0x001a: sRetVal.strTag = _T("Canon.Cs1.MaxAperture");break; // 
		case 0x001b: sRetVal.strTag = _T("Canon.Cs1.MinAperture");break; // 
		case 0x001c: sRetVal.strTag = _T("Canon.Cs1.FlashActivity");break; // Short Flash activity 
		case 0x001d: sRetVal.strTag = _T("Canon.Cs1.FlashDetails");break; // Short Flash details 
		case 0x0020: sRetVal.strTag = _T("Canon.Cs1.FocusMode");break; // Short Focus mode setting 
		default:
			sRetVal.strTag.Format(_T("Canon.Cs1.x%04X"),nSubTag);
			sRetVal.bUnknown = true;
			break;
		} // switch nSubTag
		break;

	case 0x0004:

		switch(nSubTag)
		{
		case 0x0002: sRetVal.strTag = _T("Canon.Cs2.ISOSpeed");break; // Short ISO speed used 
		case 0x0004: sRetVal.strTag = _T("Canon.Cs2.TargetAperture");break; // Short Target Aperture 
		case 0x0005: sRetVal.strTag = _T("Canon.Cs2.TargetShutterSpeed");break; // Short Target shutter speed 
		case 0x0007: sRetVal.strTag = _T("Canon.Cs2.WhiteBalance");break; // Short White balance setting 
		case 0x0009: sRetVal.strTag = _T("Canon.Cs2.Sequence");break; // Short Sequence number (if in a continuous burst) 
		case 0x000e: sRetVal.strTag = _T("Canon.Cs2.AFPointUsed");break; // Short AF point used 
		case 0x000f: sRetVal.strTag = _T("Canon.Cs2.FlashBias");break; // Short Flash bias 
		case 0x0013: sRetVal.strTag = _T("Canon.Cs2.SubjectDistance");break; // Short Subject distance (m_nImgUnits are not clear) 
		case 0x0015: sRetVal.strTag = _T("Canon.Cs2.ApertureValue");break; // Short Aperture 
		case 0x0016: sRetVal.strTag = _T("Canon.Cs2.ShutterSpeedValue");break; // Short Shutter speed 
		default:
			sRetVal.strTag.Format(_T("Canon.Cs2.x%04X"),nSubTag);
			sRetVal.bUnknown = true;
			break;
		} // switch nSubTag
		break;

	case 0x000F:

		// CustomFunctions are different! Tag given by high byte, value by low
		// Index order (usually the nSubTag) is not used.
		sRetVal.strVal.Format(_T("%u"),nValLo); // Provide default value
		switch(nValHi)
		{

		case 0x0001: sRetVal.strTag = _T("Canon.Cf.NoiseReduction");break; // Short Long exposure noise reduction 
		case 0x0002: sRetVal.strTag = _T("Canon.Cf.ShutterAeLock");break; // Short Shutter/AE lock buttons 
		case 0x0003: sRetVal.strTag = _T("Canon.Cf.MirrorLockup");break; // Short Mirror lockup 
		case 0x0004: sRetVal.strTag = _T("Canon.Cf.ExposureLevelIncrements");break; // Short Tv/Av and exposure level 
		case 0x0005: sRetVal.strTag = _T("Canon.Cf.AFAssist");break; // Short AF assist light 
		case 0x0006: sRetVal.strTag = _T("Canon.Cf.FlashSyncSpeedAv");break; // Short Shutter speed in Av mode 
		case 0x0007: sRetVal.strTag = _T("Canon.Cf.AEBSequence");break; // Short AEB sequence/auto cancellation 
		case 0x0008: sRetVal.strTag = _T("Canon.Cf.ShutterCurtainSync");break; // Short Shutter curtain sync 
		case 0x0009: sRetVal.strTag = _T("Canon.Cf.LensAFStopButton");break; // Short Lens AF stop button Fn. Switch 
		case 0x000a: sRetVal.strTag = _T("Canon.Cf.FillFlashAutoReduction");break; // Short Auto reduction of fill flash 
		case 0x000b: sRetVal.strTag = _T("Canon.Cf.MenuButtonReturn");break; // Short Menu button return position 
		case 0x000c: sRetVal.strTag = _T("Canon.Cf.SetButtonFunction");break; // Short SET button func. when shooting 
		case 0x000d: sRetVal.strTag = _T("Canon.Cf.SensorCleaning");break; // Short Sensor cleaning 
		case 0x000e: sRetVal.strTag = _T("Canon.Cf.SuperimposedDisplay");break; // Short Superimposed display 
		case 0x000f: sRetVal.strTag = _T("Canon.Cf.ShutterReleaseNoCFCard");break; // Short Shutter Release W/O CF Card 
		default:
			sRetVal.strTag.Format(_T("Canon.Cf.x%04X"),nValHi);
			sRetVal.bUnknown = true;
			break;
		} // switch nSubTag
		break;

/*
	// Other ones assumed to use high-byte/low-byte method:
	case 0x00C0:
		sRetVal.strVal.Format(_T("%u"),nValLo); // Provide default value
		switch(nValHi)
		{
			//case 0x0001: sRetVal.strTag = _T("Canon.x00C0.???");break; //
			default:
				sRetVal.strTag.Format(_T("Canon.x00C0.x%04X"),nValHi);
				break;
		}
		break;

	case 0x00C1:
		sRetVal.strVal.Format(_T("%u"),nValLo); // Provide default value
		switch(nValHi)
		{
			//case 0x0001: sRetVal.strTag = _T("Canon.x00C1.???");break; //
			default:
				sRetVal.strTag.Format(_T("Canon.x00C1.x%04X"),nValHi);
				break;
		}
		break;
*/

	case 0x0012:

		switch(nSubTag)
		{
		case 0x0002: sRetVal.strTag = _T("Canon.Pi.ImageWidth");break; //
		case 0x0003: sRetVal.strTag = _T("Canon.Pi.ImageHeight");break; //
		case 0x0004: sRetVal.strTag = _T("Canon.Pi.ImageWidthAsShot");break; //
		case 0x0005: sRetVal.strTag = _T("Canon.Pi.ImageHeightAsShot");break; //
		case 0x0016: sRetVal.strTag = _T("Canon.Pi.AFPointsUsed");break; //
		case 0x001a: sRetVal.strTag = _T("Canon.Pi.AFPointsUsed20D");break; //
		default:
			sRetVal.strTag.Format(_T("Canon.Pi.x%04X"),nSubTag);
			sRetVal.bUnknown = true;
			break;
		} // switch nSubTag
		break;

	default:
		sRetVal.strTag.Format(_T("Canon.x%04X.x%04X"),nMainTag,nSubTag);
		sRetVal.bUnknown = true;
		break;

	} // switch mainTag

	return sRetVal;
}