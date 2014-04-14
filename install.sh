sudo mkdir -p /opt/vc/lib/libegl-unpack
gcc -L/opt/vc/lib -lEGL -lGLESv2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -c k_eglGetDisplayCopy.c
cp k_eglGetDisplayCopy.o /opt/vc/lib/libegl-unpack
cd /opt/vc/lib/libegl-unpack
ar x ../libEGL_static.a
objcopy --redefine-sym eglGetDisplay=real_eglGetDisplay --redefine-sym eglCreateWindowSurface=real_eglCreateWindowSurface --redefine-sym eglSwapBuffers=real_eglSwapBuffers --redefine-sym eglMakeCurrent=real_eglMakeCurrent egl_client.c.o egl_client-modified.c.o
cd ..
gcc -shared -o libEGL.so -L/opt/vc/lib libegl-unpack/*.o -g -lbcm_host libkhrn_static.a
echo "Installation END"
