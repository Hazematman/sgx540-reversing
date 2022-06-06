/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// OpenGL ES 2.0 code

#include <jni.h>
#include <android/log.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <ios>
#include <unistd.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

auto gVertexShader =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
      "  gl_Position = vPosition;"
      "  gl_PointSize = 1.0;"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(1.0, 0.0 + gl_PointCoord.x, 0.0 + gl_PointCoord.y, 0.0);\n"
    "}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                            shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    LOGE("loading vertex shader");
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    LOGE("loaded vertex shader");
    if (!vertexShader) {
        return 0;
    }

    LOGE("loading fragment shader");
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        LOGE("attaching vertex shader to program");
        glAttachShader(program, vertexShader);
        LOGE("attached vertex shader to program");
        checkGlError("glAttachShader");
        LOGE("attaching fragment shader to program");
        glAttachShader(program, pixelShader);
        LOGE("attached fragment shader to program");
        checkGlError("glAttachShader");
        LOGE("linking program");
        glLinkProgram(program);
        LOGE("linked program");
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    LOGE("version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Get the binary length
    GLint length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, &length);

    // Retrieve the binary code
    std::vector<GLubyte> buffer(length);
    GLenum format = 0;
    LOGE("getting program binary");
    glGetProgramBinaryOES(program, length, NULL, &format, buffer.data());
    LOGE("got program binary");
    if (int i = glGetError() != 0) {
        LOGE("Error %d", i);
        return program;
    }
    LOGE("Successfully got oes program of size %x", length);
    std::ofstream out("/sdcard/shader.bin", std::ios::binary);
    out.write( reinterpret_cast<char *>(buffer.data()), length );
    out.close();
    LOGE("Program written to disk %d %d %d", buffer[0], buffer[1], buffer[2]);

    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;
GLint  aloc;
GLint  bloc;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    LOGE("getting vert attrib location");
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    LOGE("got vert attrib location");
    checkGlError("glGetAttribLocation");
    aloc = glGetUniformLocation(gProgram, "a");
    bloc = glGetUniformLocation(gProgram, "b");
    checkGlError("glGetUniformLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);

    LOGE("glviewport");
    glViewport(0, 0, w, h);
    LOGE("glviewport done");
    checkGlError("glViewport");
    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f };

void renderFrame() {
    static float grey = 0.0;
/*    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    */
    LOGE("glclearcolor");
    glClearColor(grey, grey, grey, 1.0f);
    LOGE("glclearcolor done");
    checkGlError("glClearColor");
    LOGE("glclear depth and color");
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    LOGE("glclear depth and color done");
    checkGlError("glClear");

    LOGE("gl using program");
    glUseProgram(gProgram);
    LOGE("gl used program");
    checkGlError("glUseProgram");

    LOGE("glvertexattribpointer");
    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    LOGE("glvertexattribpointer done");
    checkGlError("glVertexAttribPointer");
    glUniform1f(aloc, 0.3);
    glUniform1f(bloc, 0.3);
    checkGlError("glUniform1f");
    LOGE("glvertexattribarray");
    glEnableVertexAttribArray(gvPositionHandle);
    LOGE("glvertexattribarray done");
    checkGlError("glEnableVertexAttribArray");
    LOGE("draw arrays");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    LOGE("draw arrays done");
    checkGlError("glDrawArrays");
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
    renderFrame();
}
