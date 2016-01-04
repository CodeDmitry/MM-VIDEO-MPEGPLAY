/*

xstub.h - Interface to stub of XWindows

*/

#ifndef XSTUB_H
#define XSTUB_H

/* @@@ See comment accompanying definition of pixel array */
#ifdef OS2
#define PIXEL(p) p
#else
#define PIXEL(p) pixel[p]
#endif

/* Bits from TCP/IP headers */
unsigned long lswap(unsigned long);
#define htonl(x)   (lswap(x))
#define bzero(x,y) memset((x),'\0',(y))

typedef unsigned long unknown_t;

typedef unsigned long Display;
#define	MSBFirst 1
typedef struct { int byte_order, bitmap_bit_order, width, height; char *data; } XImage;
typedef unsigned long Colormap;
typedef unsigned long Window;
typedef unsigned long Visual;
typedef unsigned long XErrorEvent;
typedef unsigned long GC;
#define TrueColor 1
#define VisualClassMask 0xff
typedef struct { int class, depth; Visual *visual; } XVisualInfo;
typedef struct { Colormap colormap; int background_pixel, border_pixel; } XSetWindowAttributes;

#define InputOutput 1

#define DoRed   0x01
#define DoGreen 0x02
#define DoBlue  0x04
typedef struct { int red,green,blue,flags,pixel; } XColor;

typedef struct { Visual *visual; } XWindowAttributes;

#define PPosition 0x0001
#define PSize     0x0002
typedef struct { int x,y,width,height,flags; } XSizeHints;

typedef struct { int background,foreground; } XGCValues;

void XSetErrorHandler(
	int (*handler)(Display *dpy, XErrorEvent *event)
	);

Display *XOpenDisplay(char *name);

void XResizeWindow(Display *dpy, Window window, int w, int h);

void XFlush(Display *dpy);

Window XCreateSimpleWindow(
	Display *dpy,
	Window root,
	int x, int y, int width, int height, int n,
	unsigned int fg, unsigned int bg
	);

#define	PseudoColor 1
#define GrayScale   2
int XMatchVisualInfo(
	Display *dpy, int screen, int bpp,
	int type,
	XVisualInfo *vinfo
	);

Colormap XDefaultColormap(Display *dpy, int screen);

int XAllocColor(Display *display, Colormap cmap, XColor *xcolor);

void XGetWindowAttributes(
	Display *display, Window window,
	XWindowAttributes *xwa
	);

#define	AllocNone 1
Colormap XCreateColormap(
	Display *display, Window window,
	Visual *visual, int flags
	);

void XSetWindowColormap(Display *display, Window window, Colormap cmap);

#define	GCForeground 1
#define GCBackground 2
GC XCreateGC(
	Display *display, Window window,
	unknown_t flags, XGCValues *xgcv
	);

#define ZPixmap 1
#define	XYBitmap 2
#define None ((Visual *) 0)
XImage *XCreateImage(
	Display *display, Visual *visual,
	int depth, int flag1, int flag2, char *dummy,
	int w, int h, int bpp, int zero
	);

XVisualInfo *XGetVisualInfo(
	Display *dpy,
	int visualclassmask, XVisualInfo *vinfo, int *numitems
	);

#define CWBackPixel   0x01 /* for use in mask */
#define CWColormap    0x02 /* for use in mask */
#define CWBorderPixel 0x04 /* for use in mask */
Window XCreateWindow(
	Display *dpy,
	Window window,
	int x, int y, int w, int h,
	int one, int depth,
	unsigned int class,
	Visual *visual,
	unsigned int mask,
	XSetWindowAttributes *wswa
	);

void XFreeColors(
	Display *display,
	Colormap cmap,
	long *pixel,
	int one,
	int zero
	);

#define XRootWindow(dpy, screen) ((Window) 0)
#define RootWindow(dpy, screen) XRootWindow(dpy, screen)
#define XDefaultScreen(display) 0
#define	DefaultScreen(display) XDefaultScreen(display)
#define DefaultRootWindow(display) RootWindow(display, DefaultScreen(display))

#define	WhitePixel(dpy, screen) 255
#define BlackPixel(dpy, screen) 0

void XFree(void *ptr);

void XPutImage(
	Display *display, Window window, GC gc,
	XImage *ximage,
	int zero1, int zero2, int zero3, int zero4,
	int width, int height
	);
 
#endif
