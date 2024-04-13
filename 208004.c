ptrarray_length(char** arr)
{
	int i = 0;
	while(arr[i] != NULL) i++;
	return i;
}