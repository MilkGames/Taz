#include <babel.h>

//TBDebugStream * bCurrentDebugStream = (TBDebugStream*) 0x0;


char* bkDataToSafeString(uchar *data, int dataSize, char* buffer, int bufferSize){
    int sz;
	uchar* destchar = (uchar*) buffer;
	
	if(data == (uchar*) 0x0){
		*buffer = '\0';
	}
	if(dataSize < (bufferSize - 1)){
		sz = dataSize;
	} else {
		sz = bufferSize - 1;
	}
	while (0 < sz){
		if(*data < 0x20){
			*destchar = '.';
		} else {
			*destchar = *data;
		}
		++destchar;
		++data;
		--sz;
	}

    return buffer;
}

void bkSetDebugStreamFlags(TBDebugStream *stream, uint32 flagMask, uint32 newValues){

	if(stream == (TBDebugStream*)0x0){
		stream = bCurrentDebugStream;
	}
	stream->flags = stream->flags & ~flagMask | newValues & flagMask;
}