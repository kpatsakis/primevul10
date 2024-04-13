void PrintGeneralUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# General Options\n"
		"MP4Box is a multimedia packager, with a vast number of functionalities: conversion, splitting, hinting, dumping, DASH-ing, encryption and others.\n"
		"MP4Box provides a large set of options, classified by categories (see [-h]()). These options do not follow any particular ordering.\n"
		"MP4Box performs in-place rewrite of IsoMedia files (the input file is overwritten). You can change this behaviour by using the [-out]() option.\n"
		"MP4Box stores by default the file with 0.5 second interleaving and meta-data (`moov`...) at the beginning, making it suitable for HTTP streaming. This may however takes longer to store the file, use [-flat]() to change this behaviour.\n"
		"MP4Box usually generates a temporary file when creating a new IsoMedia file. The location of this temporary file is OS-dependent, and it may happen that the drive/partition the temporary file is created on has not enough space or no write access. In such a case, you can specify a temporary file location with [-tmp]().\n"
		"Note: Track operations identify tracks through their ID (usually refered as tkID in the help), not their order.\n"
		"Option values:\n"
		"Unless specified otherwise, an option of type `integer` expects a trackID value following it."
		"An option of type `boolean` expects no following value."
		"  \n"
		"# File Splitting and Concatenation\n"
		"MP4Box can split IsoMedia files by size, duration or extract a given part of the file to new IsoMedia file(s). This process requires that at most one track in the input file has non random-access points (typically one video track at most). This process will also ignore all MPEG-4 Systems tracks and hint tracks, but will try to split private media tracks.\n"
		"Note: The input file must have enough random access points in order to be split. This may not be the case with some video files where only the very first sample of the video track is a key frame (many 3GP files with H263 video are recorded that way). In order to split such files you will have to use a real video editor and re-encode the content.\n"
		"Note: You can add media to a file and split it in the same pass. In this case, the destination file (the one which would be obtained without spliting) will not be stored.\n"
		"  \n"
		"Options:\n"
	);

	while (m4b_gen_args[i].name) {
		GF_GPACArg *arg = &m4b_gen_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-gen");
	}
}