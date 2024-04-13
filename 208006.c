growPatternArray (SCPattern item)
{
	void *_tmp = NULL;
        if (pattc == current_pattern_size) {
                if (current_pattern_size == 0)
                        current_pattern_size = PATTERN_ARR_SIZE;
                else
                        current_pattern_size += PATTERN_ARR_SIZE;

		_tmp = realloc(patterns, (current_pattern_size * sizeof(SCPattern)));
                if (!_tmp) {
                        return(-1);
                }

                patterns = (SCPattern*)_tmp;
        }
        patterns[pattc] = item;
        pattc++;

	return(pattc);
}