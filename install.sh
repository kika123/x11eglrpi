sudo rm -rf /opt/vc/lib/libegl-unpack
sudo mkdir -p /opt/vc/lib/libegl-unpack
gcc -L/opt/vc/lib -lEGL -lGLESv2 -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -c k_eglGetDisplayCopy.c
sudo mv k_eglGetDisplayCopy.o /opt/vc/lib/libegl-unpack
cd /opt/vc/lib/libegl-unpack
sudo ar x ../libEGL_static.a
sudo objcopy --redefine-sym eglGetDisplay=real_eglGetDisplay --redefine-sym eglCreateWindowSurface=real_eglCreateWindowSurface --redefine-sym eglSwapBuffers=real_eglSwapBuffers --redefine-sym eglMakeCurrent=real_eglMakeCurrent egl_client.c.o egl_client-modified.c.o
cd ..
sudo gcc -shared -o libEGL.so -L/opt/vc/lib libegl-unpack/*.o -g -lbcm_host libkhrn_static.a
echo "Installation END"
