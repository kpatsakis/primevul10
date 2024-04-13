CString CjfifDecode::LookupExifTag(CString strSect,unsigned nTag,bool &bUnknown)
{
	CString strTmp;
	bUnknown = false;

	if (strSect == _T("IFD0"))
	{

		switch(nTag)
		{

		case 0x010E: return _T("ImageDescription");break; // ascii string Describes image 
		case 0x010F: return _T("Make");break; // ascii string Shows manufacturer of digicam 
		case 0x0110: return _T("Model");break; // ascii string Shows model number of digicam
		case 0x0112: return _T("Orientation");break; // unsigned short 1  The orientation of the camera relative to the scene, when the image was captured. The start point of stored data is, '1' means upper left, '3' lower right, '6' upper right, '8' lower left, '9' undefined. 
		case 0x011A: return _T("XResolution");break; // unsigned rational 1  Display/Print resolution of image. Large number of digicam uses 1/72inch, but it has no mean because personal computer doesn't use this value to display/print out. 
		case 0x011B: return _T("YResolution");break; // unsigned rational 1  
		case 0x0128: return _T("ResolutionUnit");break; // unsigned short 1  Unit of XResolution(0x011a)/YResolution(0x011b). '1' means no-unit, '2' means inch, '3' means centimeter. 
		case 0x0131: return _T("Software");break; //  ascii string Shows firmware(internal software of digicam) version number. 
		case 0x0132: return _T("DateTime");break; // ascii string 20  Date/Time of image was last modified. Data format is "YYYY:MM:DD HH:MM:SS"+0x00, total 20bytes. In usual, it has the same value of DateTimeOriginal(0x9003) 
		case 0x013B: return _T("Artist");break; // Seems to be here and not only in SubIFD (maybe instead of SubIFD)
		case 0x013E: return _T("WhitePoint");break; // unsigned rational 2  Defines chromaticity of white point of the image. If the image uses CIE Standard Illumination D65(known as international standard of 'daylight'), the values are '3127/10000,3290/10000'. 
		case 0x013F: return _T("PrimChromaticities");break; // unsigned rational 6  Defines chromaticity of the primaries of the image. If the image uses CCIR Recommendation 709 primearies, values are '640/1000,330/1000,300/1000,600/1000,150/1000,0/1000'. 
		case 0x0211: return _T("YCbCrCoefficients");break; // unsigned rational 3  When image format is YCbCr, this value shows a constant to translate it to RGB format. In usual, values are '0.299/0.587/0.114'. 
		case 0x0213: return _T("YCbCrPositioning");break; // unsigned short 1  When image format is YCbCr and uses 'Subsampling'(cropping of chroma data, all the digicam do that), defines the chroma sample point of subsampling pixel array. '1' means the center of pixel array, '2' means the datum point. 
		case 0x0214: return _T("ReferenceBlackWhite");break; // unsigned rational 6  Shows reference value of black point/white point. In case of YCbCr format, first 2 show black/white of Y, next 2 are Cb, last 2 are Cr. In case of RGB format, first 2 show black/white of R, next 2 are G, last 2 are B.
		case 0x8298: return _T("Copyright");break; // ascii string Shows copyright information
		case 0x8769: return _T("ExifOffset");break; //unsigned long 1  Offset to Exif Sub IFD
		case 0x8825: return _T("GPSOffset");break; //unsigned long 1  Offset to Exif GPS IFD
//NEW:
		case 0x9C9B: return _T("XPTitle");break;
		case 0x9C9C: return _T("XPComment");break;
		case 0x9C9D: return _T("XPAuthor");break;
		case 0x9C9e: return _T("XPKeywords");break;
		case 0x9C9f: return _T("XPSubject");break;
//NEW: The following were found in IFD0 even though they should just be SubIFD?
		case 0xA401: return _T("CustomRendered");break;
		case 0xA402: return _T("ExposureMode");break;
		case 0xA403: return _T("WhiteBalance");break;
		case 0xA406: return _T("SceneCaptureType");break;

		default:
			strTmp.Format(_T("IFD0.0x%04X"),nTag);
			bUnknown = true;
			return strTmp;
			break;
		}

	} else if (strSect == _T("SubIFD")) {

		switch(nTag)
		{
		case 0x00fe: return _T("NewSubfileType");break; //  unsigned long 1  
		case 0x00ff: return _T("SubfileType");break; //  unsigned short 1   
		case 0x012d: return _T("TransferFunction");break; //  unsigned short 3  
		case 0x013b: return _T("Artist");break; //  ascii string 
		case 0x013d: return _T("Predictor");break; //  unsigned short 1  
		case 0x0142: return _T("TileWidth");break; //  unsigned short 1  
		case 0x0143: return _T("TileLength");break; //  unsigned short 1  
		case 0x0144: return _T("TileOffsets");break; //  unsigned long 
		case 0x0145: return _T("TileByteCounts");break; //  unsigned short 
		case 0x014a: return _T("SubIFDs");break; //  unsigned long 
		case 0x015b: return _T("JPEGTables");break; //  undefined 
		case 0x828d: return _T("CFARepeatPatternDim");break; //  unsigned short 2  
		case 0x828e: return _T("CFAPattern");break; //  unsigned byte 
		case 0x828f: return _T("BatteryLevel");break; //  unsigned rational 1  
		case 0x829A: return _T("ExposureTime");break;
		case 0x829D: return _T("FNumber");break;
		case 0x83bb: return _T("IPTC/NAA");break; //  unsigned long 
		case 0x8773: return _T("InterColorProfile");break; //  undefined 
		case 0x8822: return _T("ExposureProgram");break;
		case 0x8824: return _T("SpectralSensitivity");break; //  ascii string 
		case 0x8825: return _T("GPSInfo");break; //  unsigned long 1  
		case 0x8827: return _T("ISOSpeedRatings");break;
		case 0x8828: return _T("OECF");break; //  undefined 
		case 0x8829: return _T("Interlace");break; //  unsigned short 1  
		case 0x882a: return _T("TimeZoneOffset");break; //  signed short 1  
		case 0x882b: return _T("SelfTimerMode");break; //  unsigned short 1  
		case 0x9000: return _T("ExifVersion");break;
		case 0x9003: return _T("DateTimeOriginal");break;
		case 0x9004: return _T("DateTimeDigitized");break;
		case 0x9101: return _T("ComponentsConfiguration");break;
		case 0x9102: return _T("CompressedBitsPerPixel");break;
		case 0x9201: return _T("ShutterSpeedValue");break;
		case 0x9202: return _T("ApertureValue");break;
		case 0x9203: return _T("BrightnessValue");break;
		case 0x9204: return _T("ExposureBiasValue");break;
		case 0x9205: return _T("MaxApertureValue");break;
		case 0x9206: return _T("SubjectDistance");break;
		case 0x9207: return _T("MeteringMode");break;
		case 0x9208: return _T("LightSource");break;
		case 0x9209: return _T("Flash");break;
		case 0x920A: return _T("FocalLength");break;
		case 0x920b: return _T("FlashEnergy");break; //  unsigned rational 1  
		case 0x920c: return _T("SpatialFrequencyResponse");break; //  undefined 
		case 0x920d: return _T("Noise");break; //  undefined 
		case 0x9211: return _T("ImageNumber");break; //  unsigned long 1  
		case 0x9212: return _T("SecurityClassification");break; //  ascii string 1  
		case 0x9213: return _T("ImageHistory");break; //  ascii string 
		case 0x9214: return _T("SubjectLocation");break; //  unsigned short 4  
		case 0x9215: return _T("ExposureIndex");break; //  unsigned rational 1  
		case 0x9216: return _T("TIFF/EPStandardID");break; //  unsigned byte 4  
		case 0x927C: return _T("MakerNote");break;
		case 0x9286: return _T("UserComment");break;
		case 0x9290: return _T("SubSecTime");break; //  ascii string 
		case 0x9291: return _T("SubSecTimeOriginal");break; //  ascii string 
		case 0x9292: return _T("SubSecTimeDigitized");break; //  ascii string 
		case 0xA000: return _T("FlashPixVersion");break;
		case 0xA001: return _T("ColorSpace");break;
		case 0xA002: return _T("ExifImageWidth");break;
		case 0xA003: return _T("ExifImageHeight");break;
		case 0xA004: return _T("RelatedSoundFile");break;
		case 0xA005: return _T("ExifInteroperabilityOffset");break;
		case 0xa20b: return _T("FlashEnergy  unsigned");break; // rational 1  
		case 0xa20c: return _T("SpatialFrequencyResponse");break; //  unsigned short 1  
		case 0xA20E: return _T("FocalPlaneXResolution");break;
		case 0xA20F: return _T("FocalPlaneYResolution");break;
		case 0xA210: return _T("FocalPlaneResolutionUnit");break;
		case 0xa214: return _T("SubjectLocation");break; //  unsigned short 1  
		case 0xa215: return _T("ExposureIndex");break; //  unsigned rational 1 
		case 0xA217: return _T("SensingMethod");break;
		case 0xA300: return _T("FileSource");break;
		case 0xA301: return _T("SceneType");break;
		case 0xa302: return _T("CFAPattern");break; //  undefined 1  
		case 0xa401: return _T("CustomRendered");break; // Short Custom image processing 
		case 0xa402: return _T("ExposureMode");break; // Short Exposure mode 
		case 0xa403: return _T("WhiteBalance");break; // Short White balance 
		case 0xa404: return _T("DigitalZoomRatio");break; // Rational Digital zoom ratio 
		case 0xa405: return _T("FocalLengthIn35mmFilm");break; // Short Focal length in 35 mm film 
		case 0xa406: return _T("SceneCaptureType");break; // Short Scene capture type 
		case 0xa407: return _T("GainControl");break; // Rational Gain control 
		case 0xa408: return _T("Contrast");break; // Short Contrast 
		case 0xa409: return _T("Saturation");break; // Short Saturation 
		case 0xa40a: return _T("Sharpness");break; // Short Sharpness 
		case 0xa40b: return _T("DeviceSettingDescription");break; // Undefined Device settings description 
		case 0xa40c: return _T("SubjectDistanceRange");break; // Short Subject distance range 
		case 0xa420: return _T("ImageUniqueID");break; // Ascii Unique image ID 

		default:
			strTmp.Format(_T("SubIFD.0x%04X"),nTag);
			bUnknown = true;
			return strTmp;
			break;

		}

	} else if (strSect == _T("IFD1")) {

		switch(nTag)
		{
		case 0x0100: return _T("ImageWidth");break; //  unsigned short/long 1  Shows size of thumbnail image. 
		case 0x0101: return _T("ImageLength");break; //  unsigned short/long 1  
		case 0x0102: return _T("BitsPerSample");break; //  unsigned short 3  When image format is no compression, this value shows the number of bits per component for each pixel. Usually this value is '8,8,8' 
		case 0x0103: return _T("Compression");break; //  unsigned short 1  Shows compression method. '1' means no compression, '6' means JPEG compression. 
		case 0x0106: return _T("PhotometricInterpretation");break; //  unsigned short 1  Shows the color space of the image data components. '1' means monochrome, '2' means RGB, '6' means YCbCr. 
		case 0x0111: return _T("StripOffsets");break; //  unsigned short/long When image format is no compression, this value shows offset to image data. In some case image data is striped and this value is plural. 
		case 0x0115: return _T("SamplesPerPixel");break; //  unsigned short 1  When image format is no compression, this value shows the number of components stored for each pixel. At color image, this value is '3'. 
		case 0x0116: return _T("RowsPerStrip");break; //  unsigned short/long 1  When image format is no compression and image has stored as strip, this value shows how many rows stored to each strip. If image has not striped, this value is the same as ImageLength(0x0101). 
		case 0x0117: return _T("StripByteConunts");break; //  unsigned short/long  When image format is no compression and stored as strip, this value shows how many bytes used for each strip and this value is plural. If image has not stripped, this value is single and means whole data size of image. 
		case 0x011a: return _T("XResolution");break; //  unsigned rational 1  Display/Print resolution of image. Large number of digicam uses 1/72inch, but it has no mean because personal computer doesn't use this value to display/print out. 
		case 0x011b: return _T("YResolution");break; //  unsigned rational 1  
		case 0x011c: return _T("PlanarConfiguration");break; //  unsigned short 1  When image format is no compression YCbCr, this value shows byte aligns of YCbCr data. If value is '1', Y/Cb/Cr value is chunky format, contiguous for each subsampling pixel. If value is '2', Y/Cb/Cr value is separated and stored to Y plane/Cb plane/Cr plane format. 
		case 0x0128: return _T("ResolutionUnit");break; //  unsigned short 1  Unit of XResolution(0x011a)/YResolution(0x011b). '1' means inch, '2' means centimeter. 
		case 0x0201: return _T("JpegIFOffset");break; //  unsigned long 1  When image format is JPEG, this value show offset to JPEG data stored. 
		case 0x0202: return _T("JpegIFByteCount");break; //  unsigned long 1  When image format is JPEG, this value shows data size of JPEG image. 
		case 0x0211: return _T("YCbCrCoefficients");break; //  unsigned rational 3  When image format is YCbCr, this value shows constants to translate it to RGB format. In usual, '0.299/0.587/0.114' are used. 
		case 0x0212: return _T("YCbCrSubSampling");break; //  unsigned short 2  When image format is YCbCr and uses subsampling(cropping of chroma data, all the digicam do that), this value shows how many chroma data subsampled. First value shows horizontal, next value shows vertical subsample rate. 
		case 0x0213: return _T("YCbCrPositioning");break; //  unsigned short 1  When image format is YCbCr and uses 'Subsampling'(cropping of chroma data, all the digicam do that), this value defines the chroma sample point of subsampled pixel array. '1' means the center of pixel array, '2' means the datum point(0,0). 
		case 0x0214: return _T("ReferenceBlackWhite");break; //  unsigned rational 6  Shows reference value of black point/white point. In case of YCbCr format, first 2 show black/white of Y, next 2 are Cb, last 2 are Cr. In case of RGB format, first 2 show black/white of R, next 2 are G, last 2 are B. 

		default:
			strTmp.Format(_T("IFD1.0x%04X"),nTag);
			bUnknown = true;
			return strTmp;
			break;

		}

	} else if (strSect == _T("InteropIFD")) {
		switch(nTag) {
			case 0x0001: return _T("InteroperabilityIndex");break;
			case 0x0002: return _T("InteroperabilityVersion");break;
			case 0x1000: return _T("RelatedImageFileFormat");break;
			case 0x1001: return _T("RelatedImageWidth");break;
			case 0x1002: return _T("RelatedImageLength");break;

			default:
				strTmp.Format(_T("Interop.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
		}
	} else if (strSect == _T("GPSIFD")) {
		switch(nTag) {
			case 0x0000: return _T("GPSVersionID");break;
			case 0x0001: return _T("GPSLatitudeRef");break;
			case 0x0002: return _T("GPSLatitude");break;
			case 0x0003: return _T("GPSLongitudeRef");break;
			case 0x0004: return _T("GPSLongitude");break;
			case 0x0005: return _T("GPSAltitudeRef");break;
			case 0x0006: return _T("GPSAltitude");break;
			case 0x0007: return _T("GPSTimeStamp");break;
			case 0x0008: return _T("GPSSatellites");break;
			case 0x0009: return _T("GPSStatus");break;
			case 0x000A: return _T("GPSMeasureMode");break;
			case 0x000B: return _T("GPSDOP");break;
			case 0x000C: return _T("GPSSpeedRef");break;
			case 0x000D: return _T("GPSSpeed");break;
			case 0x000E: return _T("GPSTrackRef");break;
			case 0x000F: return _T("GPSTrack");break;
			case 0x0010: return _T("GPSImgDirectionRef");break;
			case 0x0011: return _T("GPSImgDirection");break;
			case 0x0012: return _T("GPSMapDatum");break;
			case 0x0013: return _T("GPSDestLatitudeRef");break;
			case 0x0014: return _T("GPSDestLatitude");break;
			case 0x0015: return _T("GPSDestLongitudeRef");break;
			case 0x0016: return _T("GPSDestLongitude");break;
			case 0x0017: return _T("GPSDestBearingRef");break;
			case 0x0018: return _T("GPSDestBearing");break;
			case 0x0019: return _T("GPSDestDistanceRef");break;
			case 0x001A: return _T("GPSDestDistance");break;
			case 0x001B: return _T("GPSProcessingMethod");break;
			case 0x001C: return _T("GPSAreaInformation");break;
			case 0x001D: return _T("GPSDateStamp");break;
			case 0x001E: return _T("GPSDifferential");break;

			default:
				strTmp.Format(_T("GPS.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
		}
	} else if (strSect == _T("MakerIFD")) {

		// Makernotes need special handling
		// We only support a few different manufacturers for makernotes.
		
		// A few Canon tags are supported in this routine, the rest are
		// handled by the LookupMakerCanonTag() call.
		if (m_strImgExifMake == _T("Canon")) {

			switch(nTag)
			{
			case 0x0001: return _T("Canon.CameraSettings1");break;
			case 0x0004: return _T("Canon.CameraSettings2");break;
			case 0x0006: return _T("Canon.ImageType");break;
			case 0x0007: return _T("Canon.FirmwareVersion");break;
			case 0x0008: return _T("Canon.ImageNumber");break;
			case 0x0009: return _T("Canon.OwnerName");break;
			case 0x000C: return _T("Canon.SerialNumber");break;
			case 0x000F: return _T("Canon.CustomFunctions");break;
			case 0x0012: return _T("Canon.PictureInfo");break;
			case 0x00A9: return _T("Canon.WhiteBalanceTable");break;

			default:
				strTmp.Format(_T("Canon.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
			}

		} // Canon

		else if (m_strImgExifMake == _T("SIGMA"))
		{
			switch(nTag)
			{
			case 0x0002: return _T("Sigma.SerialNumber");break; // Ascii Camera serial number 
			case 0x0003: return _T("Sigma.DriveMode");break; // Ascii Drive Mode 
			case 0x0004: return _T("Sigma.ResolutionMode");break; // Ascii Resolution Mode 
			case 0x0005: return _T("Sigma.AutofocusMode");break; // Ascii Autofocus mode 
			case 0x0006: return _T("Sigma.FocusSetting");break; // Ascii Focus setting 
			case 0x0007: return _T("Sigma.WhiteBalance");break; // Ascii White balance 
			case 0x0008: return _T("Sigma.ExposureMode");break; // Ascii Exposure mode 
			case 0x0009: return _T("Sigma.MeteringMode");break; // Ascii Metering mode 
			case 0x000a: return _T("Sigma.LensRange");break; // Ascii Lens focal length range 
			case 0x000b: return _T("Sigma.ColorSpace");break; // Ascii Color space 
			case 0x000c: return _T("Sigma.Exposure");break; // Ascii Exposure 
			case 0x000d: return _T("Sigma.Contrast");break; // Ascii Contrast 
			case 0x000e: return _T("Sigma.Shadow");break; // Ascii Shadow 
			case 0x000f: return _T("Sigma.Highlight");break; // Ascii Highlight 
			case 0x0010: return _T("Sigma.Saturation");break; // Ascii Saturation 
			case 0x0011: return _T("Sigma.Sharpness");break; // Ascii Sharpness 
			case 0x0012: return _T("Sigma.FillLight");break; // Ascii X3 Fill light 
			case 0x0014: return _T("Sigma.ColorAdjustment");break; // Ascii Color adjustment 
			case 0x0015: return _T("Sigma.AdjustmentMode");break; // Ascii Adjustment mode 
			case 0x0016: return _T("Sigma.Quality");break; // Ascii Quality 
			case 0x0017: return _T("Sigma.Firmware");break; // Ascii Firmware 
			case 0x0018: return _T("Sigma.Software");break; // Ascii Software 
			case 0x0019: return _T("Sigma.AutoBracket");break; // Ascii Auto bracket 
			default:
				strTmp.Format(_T("Sigma.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
			}
		} // SIGMA

		else if (m_strImgExifMake == _T("SONY"))
		{

			switch(nTag)
			{
			case 0xb021: return _T("Sony.ColorTemperature");break;
			case 0xb023: return _T("Sony.SceneMode");break;
			case 0xb024: return _T("Sony.ZoneMatching");break;
			case 0xb025: return _T("Sony.DynamicRangeOptimizer");break;
			case 0xb026: return _T("Sony.ImageStabilization");break;
			case 0xb027: return _T("Sony.LensID");break;
			case 0xb029: return _T("Sony.ColorMode");break;
			case 0xb040: return _T("Sony.Macro");break;
			case 0xb041: return _T("Sony.ExposureMode");break;
			case 0xb047: return _T("Sony.Quality");break;
			case 0xb04e: return _T("Sony.LongExposureNoiseReduction");break;
			default:
				// No real info is known
				strTmp.Format(_T("Sony.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
			}
		} // SONY

		else if (m_strImgExifMake == _T("FUJIFILM"))
		{
			switch(nTag)
			{
			case 0x0000: return _T("Fujifilm.Version");break; // Undefined Fujifilm Makernote version 
			case 0x1000: return _T("Fujifilm.Quality");break; // Ascii Image quality setting 
			case 0x1001: return _T("Fujifilm.Sharpness");break; // Short Sharpness setting 
			case 0x1002: return _T("Fujifilm.WhiteBalance");break; // Short White balance setting 
			case 0x1003: return _T("Fujifilm.Color");break; // Short Chroma saturation setting 
			case 0x1004: return _T("Fujifilm.Tone");break; // Short Contrast setting 
			case 0x1010: return _T("Fujifilm.FlashMode");break; // Short Flash firing mode setting 
			case 0x1011: return _T("Fujifilm.FlashStrength");break; // SRational Flash firing strength compensation setting 
			case 0x1020: return _T("Fujifilm.Macro");break; // Short Macro mode setting 
			case 0x1021: return _T("Fujifilm.FocusMode");break; // Short Focusing mode setting 
			case 0x1030: return _T("Fujifilm.SlowSync");break; // Short Slow synchro mode setting 
			case 0x1031: return _T("Fujifilm.PictureMode");break; // Short Picture mode setting 
			case 0x1100: return _T("Fujifilm.Continuous");break; // Short Continuous shooting or auto bracketing setting 
			case 0x1210: return _T("Fujifilm.FinePixColor");break; // Short Fuji FinePix Color setting 
			case 0x1300: return _T("Fujifilm.BlurWarning");break; // Short Blur warning status 
			case 0x1301: return _T("Fujifilm.FocusWarning");break; // Short Auto Focus warning status 
			case 0x1302: return _T("Fujifilm.AeWarning");break; // Short Auto Exposure warning status 
			default:
				strTmp.Format(_T("Fujifilm.0x%04X"),nTag);
				bUnknown = true;
				return strTmp;
				break;
			}
		} // FUJIFILM

		else if (m_strImgExifMake == _T("NIKON"))
		{
			if (m_nImgExifMakeSubtype == 1) {
				// Type 1
				switch(nTag)
				{
				case 0x0001: return _T("Nikon1.Version");break; // Undefined Nikon Makernote version 
				case 0x0002: return _T("Nikon1.ISOSpeed");break; // Short ISO speed setting 
				case 0x0003: return _T("Nikon1.ColorMode");break; // Ascii Color mode 
				case 0x0004: return _T("Nikon1.Quality");break; // Ascii Image quality setting 
				case 0x0005: return _T("Nikon1.WhiteBalance");break; // Ascii White balance 
				case 0x0006: return _T("Nikon1.Sharpening");break; // Ascii Image sharpening setting 
				case 0x0007: return _T("Nikon1.Focus");break; // Ascii Focus mode 
				case 0x0008: return _T("Nikon1.Flash");break; // Ascii Flash mode 
				case 0x000f: return _T("Nikon1.ISOSelection");break; // Ascii ISO selection 
				case 0x0010: return _T("Nikon1.DataDump");break; // Undefined Data dump 
				case 0x0080: return _T("Nikon1.ImageAdjustment");break; // Ascii Image adjustment setting 
				case 0x0082: return _T("Nikon1.Adapter");break; // Ascii Adapter used 
				case 0x0085: return _T("Nikon1.FocusDistance");break; // Rational Manual focus distance 
				case 0x0086: return _T("Nikon1.DigitalZoom");break; // Rational Digital zoom setting 
				case 0x0088: return _T("Nikon1.AFFocusPos");break; // Undefined AF focus position 
				default:
					strTmp.Format(_T("Nikon1.0x%04X"),nTag);
					bUnknown = true;
					return strTmp;
					break;
				}
			}
			else if (m_nImgExifMakeSubtype == 2)
			{
				// Type 2
				switch(nTag)
				{
				case 0x0003: return _T("Nikon2.Quality");break; // Short Image quality setting 
				case 0x0004: return _T("Nikon2.ColorMode");break; // Short Color mode 
				case 0x0005: return _T("Nikon2.ImageAdjustment");break; // Short Image adjustment setting 
				case 0x0006: return _T("Nikon2.ISOSpeed");break; // Short ISO speed setting 
				case 0x0007: return _T("Nikon2.WhiteBalance");break; // Short White balance 
				case 0x0008: return _T("Nikon2.Focus");break; // Rational Focus mode 
				case 0x000a: return _T("Nikon2.DigitalZoom");break; // Rational Digital zoom setting 
				case 0x000b: return _T("Nikon2.Adapter");break; // Short Adapter used 
				default:
					strTmp.Format(_T("Nikon2.0x%04X"),nTag);
					bUnknown = true;
					return strTmp;
					break;
				}
			}
			else if (m_nImgExifMakeSubtype == 3)
			{
				// Type 3
				switch(nTag)
				{
				case 0x0001: return _T("Nikon3.Version");break; // Undefined Nikon Makernote version 
				case 0x0002: return _T("Nikon3.ISOSpeed");break; // Short ISO speed used 
				case 0x0003: return _T("Nikon3.ColorMode");break; // Ascii Color mode 
				case 0x0004: return _T("Nikon3.Quality");break; // Ascii Image quality setting 
				case 0x0005: return _T("Nikon3.WhiteBalance");break; // Ascii White balance 
				case 0x0006: return _T("Nikon3.Sharpening");break; // Ascii Image sharpening setting 
				case 0x0007: return _T("Nikon3.Focus");break; // Ascii Focus mode 
				case 0x0008: return _T("Nikon3.FlashSetting");break; // Ascii Flash setting 
				case 0x0009: return _T("Nikon3.FlashMode");break; // Ascii Flash mode 
				case 0x000b: return _T("Nikon3.WhiteBalanceBias");break; // SShort White balance bias 
				case 0x000e: return _T("Nikon3.ExposureDiff");break; // Undefined Exposure difference 
				case 0x000f: return _T("Nikon3.ISOSelection");break; // Ascii ISO selection 
				case 0x0010: return _T("Nikon3.DataDump");break; // Undefined Data dump 
				case 0x0011: return _T("Nikon3.ThumbOffset");break; // Long Thumbnail IFD offset 
				case 0x0012: return _T("Nikon3.FlashComp");break; // Undefined Flash compensation setting 
				case 0x0013: return _T("Nikon3.ISOSetting");break; // Short ISO speed setting 
				case 0x0016: return _T("Nikon3.ImageBoundary");break; // Short Image boundry 
				case 0x0018: return _T("Nikon3.FlashBracketComp");break; // Undefined Flash bracket compensation applied 
				case 0x0019: return _T("Nikon3.ExposureBracketComp");break; // SRational AE bracket compensation applied 
				case 0x0080: return _T("Nikon3.ImageAdjustment");break; // Ascii Image adjustment setting 
				case 0x0081: return _T("Nikon3.ToneComp");break; // Ascii Tone compensation setting (contrast) 
				case 0x0082: return _T("Nikon3.AuxiliaryLens");break; // Ascii Auxiliary lens (adapter) 
				case 0x0083: return _T("Nikon3.LensType");break; // Byte Lens type 
				case 0x0084: return _T("Nikon3.Lens");break; // Rational Lens 
				case 0x0085: return _T("Nikon3.FocusDistance");break; // Rational Manual focus distance 
				case 0x0086: return _T("Nikon3.DigitalZoom");break; // Rational Digital zoom setting 
				case 0x0087: return _T("Nikon3.FlashType");break; // Byte Type of flash used 
				case 0x0088: return _T("Nikon3.AFFocusPos");break; // Undefined AF focus position 
				case 0x0089: return _T("Nikon3.Bracketing");break; // Short Bracketing 
				case 0x008b: return _T("Nikon3.LensFStops");break; // Undefined Number of lens stops 
				case 0x008c: return _T("Nikon3.ToneCurve");break; // Undefined Tone curve 
				case 0x008d: return _T("Nikon3.ColorMode");break; // Ascii Color mode 
				case 0x008f: return _T("Nikon3.SceneMode");break; // Ascii Scene mode 
				case 0x0090: return _T("Nikon3.LightingType");break; // Ascii Lighting type 
				case 0x0092: return _T("Nikon3.HueAdjustment");break; // SShort Hue adjustment 
				case 0x0094: return _T("Nikon3.Saturation");break; // SShort Saturation adjustment 
				case 0x0095: return _T("Nikon3.NoiseReduction");break; // Ascii Noise reduction 
				case 0x0096: return _T("Nikon3.CompressionCurve");break; // Undefined Compression curve 
				case 0x0097: return _T("Nikon3.ColorBalance2");break; // Undefined Color balance 2 
				case 0x0098: return _T("Nikon3.LensData");break; // Undefined Lens data 
				case 0x0099: return _T("Nikon3.NEFThumbnailSize");break; // Short NEF thumbnail size 
				case 0x009a: return _T("Nikon3.SensorPixelSize");break; // Rational Sensor pixel size 
				case 0x00a0: return _T("Nikon3.SerialNumber");break; // Ascii Camera serial number 
				case 0x00a7: return _T("Nikon3.ShutterCount");break; // Long Number of shots taken by camera 
				case 0x00a9: return _T("Nikon3.ImageOptimization");break; // Ascii Image optimization 
				case 0x00aa: return _T("Nikon3.Saturation");break; // Ascii Saturation 
				case 0x00ab: return _T("Nikon3.VariProgram");break; // Ascii Vari program 

				default:
					strTmp.Format(_T("Nikon3.0x%04X"),nTag);
					bUnknown = true;
					return strTmp;
					break;
				}

			}
		} // NIKON

	} // if strSect

	bUnknown = true;
	return _T("???");
}