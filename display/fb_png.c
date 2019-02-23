#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <zlib.h>

#include <config.h>

#define PNG_BYTES_TO_CHECK 4

/*
 *function:charge if it is a png picture;
 *para: the path and name of the picture
 *return:1--is png, -1--error or not png
 */

int is_png(const char *path)
{
	FILE *fp;
	char buf[PNG_BYTES_TO_CHECK];
	
	/* Open the prospective PNG file. */
	if ((fp = fopen(path, "rb")) == NULL)
		return -1;
	/* Read in some of the signature bytes */
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
		return -1;
	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	Return nonzero (true) if they match */   
	return(!png_sig_cmp(buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

static int png_analyze(pic_info  *ppng)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	FILE *fp;
	png_bytep *row_pointers;
	int x, y, pos = 0;
	
	unsigned long date_len = 0;

	if ((fp = fopen(ppng->pathname, "rb")) == NULL)
		return -1;
	//Create and initialize the png_struct with the desired error handler functions.
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return -1;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{   
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return -1;
	}

	/* Set error handling if you are using the setjmp/longjmp method 
	(this is the normal method of doing things with libpng). 
	*/
	if (setjmp(png_jmpbuf(png_ptr)))
	{	  
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a problem reading the file */
		return -1;
	}

	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	/*    * If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* quantizing, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:    */
//	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND|PNG_TRANSFORM_STRIP_ALPHA, NULL);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);

	png_get_IHDR(png_ptr, info_ptr, &width, &height,
				&bit_depth, &color_type, &interlace_type,
				NULL, NULL);
	
	ppng->width = width;
	ppng->height = height;
	ppng->bpp= bit_depth;
	date_len = width * height * bit_depth / 8;
		
	debug("the bnp picture: width = %d,height = %d, bit_depth = %d,interlace_type = %d, color_type = %d\n",
		width, height, bit_depth, interlace_type, color_type);

	row_pointers = png_get_rows(png_ptr, info_ptr);

	//ppng->pic_date = row_pointers;
	//memcpy(ppng->pic_date, row_pointers, date_len);
	//!!!!!!!!!注意思考为什么通过下面这样可以，而通过上面的两种方式不可以?
	for (y = 0; y < ppng->height; y++)
			{
				for(x = 0; x < ppng->width * 3; x += 3)
				{
					ppng->pic_date[pos++] = row_pointers[y][x+0];
					ppng->pic_date[pos++] = row_pointers[y][x+1];
					ppng->pic_date[pos++] = row_pointers[y][x+2];
				}
			}
	
	fb_draw_png(ppng);
	
	/* Clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	/* Close the file */
	fclose(fp);

	return 0;

}


int png_display(const char *path)
{
	pic_info p_picture;
	int ret;
	
	if(NULL == path)
	{
		fprintf(stderr, "uselee pathname\n");
		return -1;
	}

	if((is_png(path)) != 1)
	{
		fprintf(stderr, "%s is not a png picture.\n", path);
		return -1;
	}
	
	debug("%s is a png picture.\n", path);
	
	p_picture.pathname = path;
	p_picture.pic_date= rgb_buf;

	if((png_analyze(&p_picture)) == -1)
		return -1;

	return 0;
	
}













