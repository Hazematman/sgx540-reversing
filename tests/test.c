#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

#ifdef EGL
#incude <EGL/egl.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#define ECHK(x) { \
    printf(">>> %s\n", #x); \
    EGLBoolean status = x; \
    if(!status) { \
        EGLint err = eglGetError(); \
        printf("%s: failed: %d", #x, err); \
        assert(false); \
    } \
}

#define GCHK(x) { \
    printf(">>> %s\n", #x); \
    x; \
    GLenum err = glGetError(); \
    if(err != GL_NO_ERROR) { \
        printf("%s: failed: %d", #x, err); \
        assert(false); \
    } \
}

#define FLOG(x) printf(">>> %s\n", #x), x

#ifdef EGL
static EGLint const config_attribute_list[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_NONE
};

static const EGLint context_attribute_list[] = {
//    EGL_CONTEXT_MAJOR_VERSION, 2,
//    EGL_CONTEXT_MINOR_VERSION, 0,
    EGL_NONE
};

static EGLDisplay display;
static EGLConfig config;
static EGLint num_config;
static EGLContext context;
static EGLSurface surface;
static GLint width = 256, height = 256;
#endif

static int dbl_buf[] = {
    GLX_RGBA,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_DOUBLEBUFFER,
    None,
};

Display *dpy;
Window win;
int width=256, height=256;
static uint8_t data[256*256*3];

void open_x11(int argc, char *argv[]) {
    XVisualInfo *vi = NULL;
    Colormap cmap;
    XSetWindowAttributes swa;
    GLXContext cx;
    XEvent event;
    Bool needsRedraw = False, recalcModelView = True;
    int dummy;

    if(!(dpy = XOpenDisplay(NULL))) {
        printf("Could not open display\n");
        assert(false);
    }

    if(!glXQueryExtension(dpy, &dummy, &dummy)) {
        printf("Failed to query extensions\n");
        assert(false);
    }

    if(!(vi = glXChooseVisual(dpy, DefaultScreen(dpy), dbl_buf))) {
        printf("Failed to open double buffer\n");
        assert(false);
    }

    if(!(cx = glXCreateContext(dpy, vi,
            None,
            True))) {
        printf("Failed to create context\n");
        assert(false);
    }

    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
    swa.colormap = cmap;
    swa.border_pixel = 0;
    swa.event_mask = ExposureMask | ButtonPressMask | StructureNotifyMask;
    win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, width, height,
                        0, vi->depth, InputOutput, vi->visual,
                        CWBorderPixel | CWColormap | CWEventMask,
                        &swa);

    XSetStandardProperties(dpy, win, "glx", "glx", None, argv, argc, NULL);
    glXMakeCurrent(dpy, win, cx);
    XMapWindow(dpy, win);
}

int main(int argc, char *argv[]) {
#ifdef EGL
    EGLNativeDisplayType native_dpy = EGL_DEFAULT_DISPLAY;
    EGLint egl_major, egl_minor;
    printf("Hello World!\n");
    display = eglGetDisplay(native_dpy);

    if(display == EGL_NO_DISPLAY) {
        printf("Failed to open display (0x%x)\n", eglGetError());
        return 0;
    }

    ECHK(eglInitialize(display, &egl_major, &egl_minor));
    ECHK(eglChooseConfig(display, config_attribute_list, &config, 1, &num_config));

    EGLint pbuffer_attribute_list[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_LARGEST_PBUFFER, EGL_TRUE,
        EGL_NONE
    };
    surface = eglCreatePbufferSurface(display, config, pbuffer_attribute_list);
    if(surface == EGL_NO_SURFACE) {
        printf("Failed to create pbuffer surface (0x%x)\n", eglGetError());
        return 0;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribute_list);
    if(context == EGL_NO_CONTEXT) {
        printf("Failed to create context (0x%x)\n", eglGetError());
        return 0;
    }

    ECHK(eglMakeCurrent(display, surface, surface, context));

    ECHK(eglQuerySurface(display, surface, EGL_WIDTH, &width));
    ECHK(eglQuerySurface(display, surface, EGL_HEIGHT, &height));
    printf("Buffer %dx%d\n", width, height);
#endif 
    open_x11(argc, argv);

    GCHK(glClearColor(1.0f, 0.0f, 0.0f, 0.0f));
    GCHK(glViewport(0, 0, width, height));
    GCHK(glScissor(0, 0, width, height));
    GCHK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

#if 0
    glFinish();
    if(!eglSwapBuffers(display, surface)){
        printf("Failed to swap buffers (0x%x)\n", eglGetError());
        return 0;
    }
#endif
    //GCHK(glFinish());
    //GCHK(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data));

    FLOG(glXSwapBuffers(dpy, win));

#if 0
    FILE *fp = fopen("out.ppm", "w");
    fprintf(fp, "P3\n256 256\n255\n");
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++) {
            uint8_t red = data[(y*width + x)*3 + 0];
            uint8_t green = data[(y*width + x)*3 + 1];
            uint8_t blue = data[(y*width + x)*3 + 2];
            fprintf(fp, "%d %d %d\n", red, green, blue);
        }
    }
    fclose(fp);
#endif

    return 0;
}
