#include <EGL/egl.h>
#include <EGL/eglext_brcm.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
/*  This file is part of X11EGLRPI
Copyright (C)2014 Mohamed MEDIOUNI

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
Window* root;
Window* window;
EGLDisplay *edisplay;
int width, height,depth;
Display* display;
EGLSurface egl_surface;
XWindowAttributes *window_attributes_return;
EGLDisplay eglGetDisplay(NativeDisplayType native_display) {
	puts("Getting an X11 EGL Display.");
	bcm_host_init();
	display = (Display*)native_display;
	root = DefaultRootWindow(display);
	edisplay = real_eglGetDisplay(EGL_DEFAULT_DISPLAY);
	return edisplay;
}
EGLSurface eglCreateWindowSurface(EGLDisplay egldisplay, EGLConfig config, NativeWindowType native_window, EGLint const * attrib_list) {
	window = (Window*) native_window;
	puts("Getting window information...");
	XWindowAttributes window_attributes;
	XGetWindowAttributes(display,window,&window_attributes);
	printf("Window Location: %i,%i \n Window Dimensions %i x %i \n Bit depth : %i \n",window_attributes.x,window_attributes.y,window_attributes.width,window_attributes.height,window_attributes.depth);
	 width=window_attributes.width ;
	 height=window_attributes.height;
	 depth=window_attributes.depth;
	EGLint attr[] = { // some attributes to set up our egl-interface
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_SURFACE_TYPE,
                EGL_PIXMAP_BIT | EGL_OPENGL_ES2_BIT,
                EGL_NONE
        };

        EGLConfig ecfg;
        EGLint num_config;
        if (!eglChooseConfig(edisplay, attr, &ecfg, 1, &num_config)) {
                fprintf(stderr, "Failed to choose config (eglError: %s)\n");
                return EGL_NO_SURFACE;
        }
	EGLint pixel_format = EGL_PIXEL_FORMAT_ARGB_8888_BRCM;
        EGLint rt;
        eglGetConfigAttrib(edisplay, ecfg, EGL_RENDERABLE_TYPE, &rt);

        if (rt & EGL_OPENGL_ES_BIT) {
                pixel_format |= EGL_PIXEL_FORMAT_RENDER_GLES_BRCM;
                pixel_format |= EGL_PIXEL_FORMAT_GLES_TEXTURE_BRCM;
        }
        if (rt & EGL_OPENGL_ES2_BIT) {
                pixel_format |= EGL_PIXEL_FORMAT_RENDER_GLES2_BRCM;
                pixel_format |= EGL_PIXEL_FORMAT_GLES2_TEXTURE_BRCM;
        }
        if (rt & EGL_OPENVG_BIT) {
                pixel_format |= EGL_PIXEL_FORMAT_RENDER_VG_BRCM;
                pixel_format |= EGL_PIXEL_FORMAT_VG_IMAGE_BRCM;
        }
        if (rt & EGL_OPENGL_BIT) {
                pixel_format |= EGL_PIXEL_FORMAT_RENDER_GL_BRCM;
        }
	EGLint pixmap[5];
        pixmap[0] = 0;
        pixmap[1] = 0;
        pixmap[2] = width;
        pixmap[3] = height;
        pixmap[4] = pixel_format;
	eglCreateGlobalImageBRCM(width, height, pixmap[4], 0, width*4, pixmap);
	egl_surface = eglCreatePixmapSurface(edisplay, ecfg, pixmap, 0);
	puts("EGL Surface Created");
	EGLint ctxattr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL_NONE
        };
	EGLContext context = eglCreateContext ( edisplay, ecfg, EGL_NO_CONTEXT, ctxattr );
	real_eglMakeCurrent(edisplay, egl_surface, egl_surface, context);
	return egl_surface;
}
EGLBoolean eglSwapBuffers(EGLDisplay edisplay, EGLSurface egsurface) {
	 unsigned int *buffer = (unsigned int *)malloc(height * width * 4);
         glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	 static GC gc;
	 static int status =0;
	 static XImage *image =0;
         static Pixmap pixmap;
	 static XGCValues gcvalues;
	 if (status == 0 ) {	
	 	gc = XCreateGC(display, window,0,&gcvalues);
		image = XGetImage(display, window, 0, 0, width, height, AllPlanes, ZPixmap);
		status = 1;
	 }
	 XImage *img = XCreateImage(display,CopyFromParent,24,ZPixmap,0,buffer,width, height, 32, 0);
	 XPutImage(display, window, gc, img, 0, 0, 0, 0, width, height);
	 free(buffer);
	 return EGL_TRUE;
}
EGLBoolean eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context) {
	real_eglMakeCurrent(display,draw,read,context);
	return EGL_TRUE;
}

