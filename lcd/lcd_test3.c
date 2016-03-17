#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <sys/ioctl.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <errno.h>
#include    <fcntl.h>
#include    <sys/mman.h>
#include    <linux/fb.h>
#include    "jpeglib.h"
#include    "jerror.h"

#define FB_DEV  "/dev/fb0"
#define __fnc__ __FUNCTION__

#define debug           0
#define debug_printf    0
#define BYREAD          0
#define BYMEM           1

/* function deciaration */

void usage(char *msg);
unsigned short RGB888toRGB565(unsigned char red,
        unsigned char green, unsigned char blue);
int fb_open(char *fb_device);
int fb_close(int fd);
int fb_stat(int fd, unsigned int *width, unsigned int *height, unsigned int *    depth);
void *fb_mmap(int fd, unsigned int screensize);
void *fd_mmap(int fd, unsigned int filesize);
int fb_munmap(void *start, size_t length);
int fb_pixel(void *fbmem, int width, int height,
        int x, int y, unsigned short color);

#if(debug)
void draw(unsigned char *fbp,
        struct fb_var_screeninfo vinfo,
        struct fb_fix_screeninfo finfo);
#endif

/* function implementation */

int main(int argc, char **argv)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
#if(BYREAD)
    FILE *infile;
#endif
    int fd;
    unsigned char *buffer;
    struct stat st;

    int fbdev;
    char *fb_device;
    unsigned char *fbmem;
    unsigned char *fdmem;
    unsigned int screensize;
    unsigned int fb_width;
    unsigned int fb_height;
    unsigned int fb_depth;
    register unsigned int x;
    register unsigned int y;

    /* check auguments */
    if (argc != 2) {
        usage("insuffient auguments");
        exit(-1);
    }

    /* open framebuffer device */
    if ((fb_device = getenv("FRAMEBUFFER")) == NULL)
        fb_device = FB_DEV;
    fbdev = fb_open(fb_device);

    /* get status of framebuffer device */
    fb_stat(fbdev, &fb_width, &fb_height, &fb_depth);

    /* map framebuffer device to shared memory */
    screensize = fb_width * fb_height * fb_depth / 8;
    fbmem = fb_mmap(fbdev, screensize);

#if (BYREAD)
    /* open input jpeg file */
    if ((infile = fopen(argv[1], "rb")) == NULL) {
        fprintf(stderr, "open %s failed\n", argv[1]);
        exit(-1);
    }
#endif

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open");
        exit(-1);
    }

    if (fstat(fd, &st) < 0) {
        perror("fstat");
        exit(-1);
    }

    fdmem = fd_mmap(fd, st.st_size);

    /* init jpeg decompress object error handler */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* bind jpeg decompress object to infile */
#if (BYREAD)
    jpeg_stdio_src(&cinfo, infile);
#endif

#if (BYMEM)
    jpeg_mem_src(&cinfo, fdmem, st.st_size);
#endif

    /* read jpeg header */
    jpeg_read_header(&cinfo, TRUE);

    /* decompress process */
    jpeg_start_decompress(&cinfo);
    if ((cinfo.output_width > fb_width) ||
            (cinfo.output_height > fb_height)) {
        printf("too large jpeg file, can't display\n");
#if (0)
        return -1;
#endif
    }

    buffer = (unsigned char *) malloc(cinfo.output_width *
            cinfo.output_components);

    struct fb_fix_screeninfo fb_finfo;
    struct fb_var_screeninfo fb_vinfo;

    if (ioctl(fbdev, FBIOGET_FSCREENINFO, &fb_finfo)) {
        perror(__fnc__);
        return -1;
    }

    if (ioctl(fbdev, FBIOGET_VSCREENINFO, &fb_vinfo)) {
        perror(__fnc__);
        return -1;
    }

#if(debug)
    draw(fbmem, fb_vinfo, fb_finfo);
#endif
    y = 0;
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        if (fb_depth == 16) {
            unsigned short color;

            for (x = 0; x < cinfo.output_width; x++) {
                color =
                    RGB888toRGB565(buffer[x * 3],
                            buffer[x * 3 + 1], buffer[x * 3 + 2]);
                fb_pixel(fbmem, fb_width, fb_height, x, y, color);
            }
        } else if (fb_depth == 24) {
            memcpy((unsigned char *) fbmem + y * fb_width * 3,
                    buffer, cinfo.output_width * cinfo.output_components);
        } else if (fb_depth == 32) {
            // memcpy((unsigned char *) fbmem + y * fb_width * 4,
                    // buffer, cinfo.output_width * cinfo.output_components);
            for (x = 0; x < cinfo.output_width; x++) {
                * (fbmem + y * fb_width * 4 + x * 4)     = (unsigned char)       buffer[x * 3 + 2];
                * (fbmem + y * fb_width * 4 + x * 4 + 1) = (unsigned char)       buffer[x * 3 + 1];
                * (fbmem + y * fb_width * 4 + x * 4 + 2) = (unsigned char)       buffer[x * 3 + 0];
                * (fbmem + y * fb_width * 4 + x * 4 + 3) = (unsigned char) 0;
            }
        }
        y++;    // next scanline
    }

    /* finish decompress, destroy decompress object */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    /* release memory buffer */
    free(buffer);

#if (BYREAD)
    /* close jpeg inputing file */
    fclose(infile);
#endif

    /* unmap framebuffer's shared memory */
    fb_munmap(fbmem, screensize);

#if (BYMEM)
    munmap(fdmem, (size_t) st.st_size);
    close(fd);
#endif

    /* close framebuffer device */
    fb_close(fbdev);

    return 0;
}

