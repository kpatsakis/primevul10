static char *dirpath(char *dir, const char *path){
	static char buf[SC_MAX_PATH_STRING_SIZE];

	strlcpy(buf,dir,sizeof buf);
	strlcat(buf,path,sizeof buf);
	return buf;
}