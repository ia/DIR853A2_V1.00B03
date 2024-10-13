#include <stdio.h>



static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/* Decode 4 '6-bit' characters into 3 8-bit binary bytes */
static inline void ILibdecodeblock( unsigned char in[4], unsigned char out[3] )
{
	out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
	out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
	out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}


/*! \fn Base64_Decode(unsigned char* input, const int inputlen, unsigned char** output)
 * 	\brief Decode a base64 encoded stream discarding padding, line breaks and noise
 * 		\para
 * 			\b Note: The decoded stream must be freed
 * 				\param input The stream to decode
 * 					\param inputlen The length of \a input
 * 						\param output The decoded stream
 * 							\returns The length of the decoded stream
 **/

int Base64_Decode(unsigned char* input, const int inputlen, unsigned char** output)
{
	unsigned char* inptr;
	unsigned char* out;
	unsigned char v;
	unsigned char in[4];
	int i, len;

	if (input == NULL || inputlen == 0)
	{
		*output = NULL;
		return 0;
	}

	*output = (unsigned char*)malloc(((inputlen * 3) / 4) + 4);
	out = *output;
	inptr = input;

	while( inptr <= (input+inputlen) )
	{
		for( len = 0, i = 0; i < 4 && inptr <= (input+inputlen); i++ )
		{
			v = 0;
			while( inptr <= (input+inputlen) && v == 0 ) {
				v = (unsigned char) *inptr;
				inptr++;
				v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
				if( v ) {
					v = (unsigned char) ((v == '$') ? 0 : v - 61);
				}
			}
			if( inptr <= (input+inputlen) ) {
				len++;
				if( v ) {
					in[ i ] = (unsigned char) (v - 1);
				}
			}
			else {
				in[i] = 0;
			}
		}
		if( len )
		{
			ILibdecodeblock( in, out );
			out += len-1;
		}
	}
	*out = 0;
	return (int)(out-*output);
}

int main(int argc, char **argv)
{
	unsigned char *output;
	unsigned char data[4096];
	FILE *file;
	int len;
	int ret;

	file = fopen(argv[1], "rb");
	if (file == NULL)
		return -1;
	memset((void *)data, 0, sizeof(data));
	len = fread(data, 1, sizeof(data), file);
	fclose(file);
	ret = Base64_Decode(data, len, &output);
	printf("%s\n", output);
	printf("ret = %d\n", ret);
	return 0;
}


