/*

xstub.c - Stub for XWindows

@@@ Andy Key

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define	XSTUB_C
#include "xstub.h"

/*...simplementation:0:*/
#define INCL_BASE
#include <os2.h>

#define	SCN_W 320
#define SCN_H 200
static VIOMODEINFO vmiOld;
static unsigned char *ptr;
static int scn_w, scn_h;
static unsigned char palette[0x100][3];
static int npalette;
static int data_depth;

/*...ssetpalette:0:*/
static void setpalette(BYTE *palette, int first, int num)
	{
	BYTE *buf = (BYTE *) palette;
	BYTE *_Seg16 buf1616 = (BYTE *_Seg16) buf;
	VIOCOLORREG viocreg;
	viocreg.cb = sizeof(VIOCOLORREG);
	viocreg.type = 3; /* Apparently no #define for this */
	viocreg.firstcolorreg = first;
	viocreg.numcolorregs = num;

	/* Note: viocreg.colorregaddr should be of type CHAR *_Seg16
	   But it isn't! Hence if I say viocreg.colorregaddr = buf,
	   then the correct thunking will not be applied! */
	memcpy(&viocreg.colorregaddr, &buf1616, 4);

	VioSetState(&viocreg, (HVIO) 0);
	}
/*...e*/
/*...sscn_init:0:*/
static void scn_init(void)
	{
	VIOMODEINFO vmi;
	VIOPHYSBUF phys;
	VIOOVERSCAN vioos;
	unsigned char *_Seg16 ptr1616;
	unsigned char status;

	vmi.cb     = 12;
	vmi.fbType = 3;
	vmi.color  = 8;
	vmi.col    = 40;
	vmi.row    = 25;
	vmi.hres   = SCN_W;
	vmi.vres   = SCN_H;
	VioGetMode(&vmiOld, (HVIO) 0);
	if ( VioSetMode(&vmi, (HVIO) 0) )
		{
		fprintf(stderr, "Unable to enter 320x200 at 8pp graphics mode\n");
		exit(1);
		}
	phys.cb = 0x10000;
	phys.pBuf = (unsigned char *) 0xa0000;
	if ( VioGetPhysBuf(&phys, 0) )
		{
		fprintf(stderr, "Unable to access to physical graphics screen\n");
		exit(1);
		}
	ptr1616 = (unsigned char *_Seg16) ( phys.asel[0] << 16 );
	ptr = (unsigned char *) ptr1616;
	VioScrLock(VP_WAIT, &status, (HVIO) 0);

	/* Set all palette entrys to black */
	memset(palette, 0, sizeof(palette));
	/* Ensure dish them out from 0 onwards */
	npalette = 0;

	setpalette((BYTE *) palette, 0, 0x100);

	/* Set screen to colour 255, which will get set to black */
	memset(ptr, 255, SCN_W*SCN_H);

	/* Set border colour to colour 255 */
	vioos.cb = sizeof(VIOOVERSCAN);
	vioos.type = 1; /* Apparently no #define for this */
	vioos.color = 255;
	VioSetState(&vioos, (HVIO) 0);
	}
/*...e*/
/*...sscn_term:0:*/
static void scn_term(void)
	{
	VioScrUnLock((HVIO) 0);
	VioSetMode(&vmiOld, (HVIO) 0);
	}
/*...e*/
/*...sscn_setdatadepth:0:*/
static void scn_setdatadepth(int depth)
	{
	data_depth = depth;
	}
/*...e*/
/*...sscn_setsize:0:*/
static void scn_setsize(int w, int h)
	{
	scn_w = w;
	scn_h = h;
	}
/*...e*/
/*...sscn_setpalette:0:*/
static int scn_setpalette(
	unsigned char r, unsigned char g, unsigned char b
	)
	{
	palette[npalette][0] = r;
	palette[npalette][1] = g;
	palette[npalette][2] = b;
	return npalette++;
	}
/*...e*/
/*...sscn_putimage:0:*/
/*
We are being asked to transfer a w*h image to a SCN_W*SCN_H screen.
If image is larger than the screen, then we must clip it.
Also, if image is smaller than screen, then we will centralise it.
*/

static int nopalette = 1;
/*...sscn_putimage8:0:*/
static void scn_putimage8(int w, int h, unsigned char *data)
	{
	int stride = w, xi, yi, y;
	unsigned char *scn;
	if ( nopalette )
		{
		setpalette((BYTE *) palette, 0, 0x100);
		nopalette = 0;
		}
	if ( w > SCN_W )
		{
		int extra = w - SCN_W;
		data += extra/2;
		w = SCN_W;
		}
	if ( h > SCN_H )
		{
		int extra = h - SCN_H;
		data += ((extra/2) * stride);
		h = SCN_H;
		}
	xi = (SCN_W-w)/2;
	yi = (SCN_H-h)/2;
	scn = ptr + ((yi*SCN_W) + xi);
	for ( y = 0; y < h; y++, data += stride, scn += SCN_W )
		memcpy(scn, data, w);
	}
/*...e*/
/*...sscn_putimage1:0:*/
/*...sbitexpand:0:*/
static void bitexpand(
	unsigned char *scn,
	unsigned char *data,
	int bitindex,
	int w
	)
	{
	int x;
	for ( x = 0; x < w; x++, bitindex++ )
		*scn++ = ( (data[bitindex>>3]&(0x80>>(bitindex&7))) != 0 ) ? 1 : 0;
	}
/*...e*/

static void scn_putimage1(int w, int h, unsigned char *data)
	{
	int bitindex = 0;
	int stride = w, xi, yi, y;
	unsigned char *scn;
	if ( nopalette )
		{
		palette[1][0] = 63;
		palette[1][1] = 63;
		palette[1][2] = 63;
		npalette = 2;
		setpalette((BYTE *) palette, 0, 0x100);
		nopalette = 0;
		}
	if ( w > SCN_W )
		{
		int extra = w - SCN_W;
		bitindex += extra/2;
		w = SCN_W;
		}
	if ( h > SCN_H )
		{
		int extra = h - SCN_H;
		bitindex += ((extra/2) * stride);
		h = SCN_H;
		}
	xi = (SCN_W-w)/2;
	yi = (SCN_H-h)/2;
	scn = ptr + ((yi*SCN_W) + xi);
	for ( y = 0; y < h; y++, bitindex += stride, scn += SCN_W )
		bitexpand(scn, data, bitindex, w);
	}
/*...e*/

static void scn_putimage(int w, int h, unsigned char *data)
	{
	/* Sometimes we decode more than the size of the 'screen' */
	if ( h > scn_h ) h = scn_h;
	switch ( data_depth )
		{
		case 8:		scn_putimage8(w, h, data);	break;
		case 1:		scn_putimage1(w, h, data);	break;
		}
	}
/*...e*/
/*...e*/
/*...sx calls:0:*/
/*...slswap:0:*/
unsigned long lswap(unsigned long l)
	{
	return ( ( (l&0xff000000) >> 24 ) |
		 ( (l&0x00ff0000) >>  8 ) |
		 ( (l&0x0000ff00) <<  8 ) |
		 ( (l&0x000000ff) << 24 ) );
	}
/*...e*/
/*...sXSetErrorHandler:0:*/
void XSetErrorHandler(
	int (*handler)(Display *dpy, XErrorEvent *event)
	) { /* Ignore */ }
/*...e*/
/*...sXOpenDisplay:0:*/
static Display d = (Display) 0;

Display *XOpenDisplay(char *name)
	{
	scn_init();
	return &d;
	}
/*...e*/
/*...sXFlush:0:*/
void XFlush(Display *dpy) { /* Ignore */ }
/*...e*/
/*...sXMatchVisualInfo:0:*/
int XMatchVisualInfo(
	Display *dpy, int screen, int bpp,
	int type,
	XVisualInfo *vinfo
	)
	{
	return 1; /* Did it! */
	}
/*...e*/
/*...sXAllocColor:0:*/
int XAllocColor(Display *display, Colormap cmap, XColor *xcolor)
	{
	xcolor->pixel = scn_setpalette(
		xcolor->red>>10, xcolor->green>>10, xcolor->blue>>10);
	return 1;
	}
/*...e*/
/*...sXFreeColors:0:*/
void XFreeColors(
	Display *display,
	Colormap cmap,
	long *pixel,
	int one,
	int zero
	)
	{
	}
/*...e*/
/*...sXDefaultColormap:0:*/
Colormap XDefaultColormap(Display *dpy, int screen)
	{
	/* Write me! */
	}
/*...e*/
/*...sXCreateColormap:0:*/
Colormap XCreateColormap(
	Display *display, Window window,
	Visual *visual, int flags
	)
	{
	return (Colormap) 0;
	}
/*...e*/
/*...sXSetWindowColormap:0:*/
void XSetWindowColormap(Display *display, Window window, Colormap cmap)
	{
	}
/*...e*/
/*...sXCreateGC:0:*/
GC XCreateGC(
	Display *display, Window window,
	unknown_t flags, XGCValues *xgcv
	)
	{
	return (GC) 0;
	}
/*...e*/
/*...sXCreateImage:0:*/
/* bpp is 8 or 32 */
/* flag1 is ZPixmap (usually) or XYBitmap (when mono) */

XImage *XCreateImage(
	Display *display, Visual *visual,
	int depth, int flag1, int flag2, char *dummy,
	int w, int h, int bpp, int zero
	)
	{
	XImage *xi;

	if ( (xi = malloc(sizeof(XImage))) == NULL )
		return (XImage *) 0;
	if ( (xi->data = malloc(w * h * depth/8)) == NULL )
		{ free(xi); return (XImage *) 0; }
	memset(xi->data, 0, w*h);
	xi->width  = w;
	xi->height = h;
	xi->bitmap_bit_order = MSBFirst;
	xi->byte_order = MSBFirst;
	scn_setdatadepth(depth);
	return xi;
	}
/*...e*/
/*...sXGetVisualInfo:0:*/
XVisualInfo *XGetVisualInfo(
	Display *dpy,
	int visualclassmask, XVisualInfo *vinfo, int *numitems
	)
	{
	*numitems = 0;
	return (XVisualInfo *) 0;
	}
/*...e*/
/*...sXCreateSimpleWindow:0:*/
Window XCreateSimpleWindow(
	Display *dpy,
	Window root,
	int x, int y, int w, int h, int n,
	unsigned int fg, unsigned int bg
	)
	{
	}
/*...e*/
/*...sXCreateWindow:0:*/
Window XCreateWindow(
	Display *dpy,
	Window window,
	int x, int y, int w, int h,
	int one, int depth,
	unsigned int class,
	Visual *visual,
	unsigned int mask,
	XSetWindowAttributes *wswa
	)
	{
	return (Window) 0;
	}
/*...e*/
/*...sXResizeWindow:0:*/
void XResizeWindow(Display *dpy, Window window, int w, int h)
	{
	scn_setsize(w, h);
	}
/*...e*/
/*...sXGetWindowAttributes:0:*/
void XGetWindowAttributes(
	Display *display, Window window,
	XWindowAttributes *xwa
	)
	{
	}
/*...e*/
/*...sXFree:0:*/
/* Used to free result from XGetVisualInfo */
void XFree(void *ptr)
	{
	}
/*...e*/
/*...sXPutImage:0:*/
void XPutImage(
	Display *display, Window window, GC gc,
	XImage *ximage,
	int zero1, int zero2, int zero3, int zero4,
	int w, int h
	)
	{
	scn_putimage(ximage->width,ximage->height,(unsigned char *) ximage->data);
	}
/*...e*/
/*...e*/
