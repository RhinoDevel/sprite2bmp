
// MT, 2020sep17

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

// Foreground pixels' color:
//
static unsigned char const fg_red = 0;
static unsigned char const fg_green = 0xFF;
static unsigned char const fg_blue = 0;

// Background pixels' color:
//
static unsigned char const bg_red = 0;
static unsigned char const bg_green = 0;
static unsigned char const bg_blue = 0;

int main(int argc, char* argv[])
{
    off_t size = -1;
    unsigned char * sprite_ptr = NULL;
    int bit = 8;

    if(argc!=3)
    {
        Deb_line(
            "Error: Exactly two arguments (src. & dest. files) must be given!")
        return 1;
    }

    unsigned char * const sprite = FileSys_loadFile(argv[1], &size);

    if(sprite==NULL)
    {
        return 2;
    }
    assert((off_t)((int)size)==size);

    Sys_log_line(true, true, "Sprite file size = %d bytes.", (int)size);

    // - 21 x 3 bytes = 63 bytes.
    // - 63 bytes x 8 <=> 504 pixels.
    //
    // => Bitmap dimensions' length = 24 x 21 pixels =  504 pixels.

    struct Bmp * const b = Bmp_create(24, 21);

    Sys_log_line(
        true, true, "Bitmap resolution = %d x %d pixels.", b->d.w, b->d.h);
        
    sprite_ptr = sprite;
    for(int row = 0;row < b->d.h;++row)
    {
        unsigned char * const row_ptr = b->p + 3 * row * b->d.w;

        for(int col = 0;col < b->d.w;++col)
        {
            unsigned char * const col_ptr = row_ptr + 3 * col;

            --bit;
            if(bit == -1)
            {
                assert(col % 8 == 0);

                ++sprite_ptr;
                bit = 7;
            }

            // Find out, if bit represents a foreground or background pixel:

            int const bit_val = ((int)(*sprite_ptr)>>bit)&1;
            bool const is_background = bit_val==0;

            // Set output pixel's channel values:

            *(col_ptr+2) = is_background?bg_red:fg_red;
            *(col_ptr+1) = is_background?bg_green:fg_green;
            *(col_ptr+0) = is_background?bg_blue:fg_blue;
        }
    }

    Bmp_save(b, argv[2]);

    free(sprite);
    Bmp_delete(b);
    return 0;
}
