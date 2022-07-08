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
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <ios>
#include <unistd.h>
#include <cstdio>
#include <signal.h>

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
        while (1) {}
    }
}

//next step: try to create a dependent chain of adds and see where that takes me

auto gVertexShader =
    "attribute vec4 vPosition;\n"

    "void main() {\n"
      "  gl_Position = vPosition;"
    "}\n";

auto gFragmentShader =
    "precision mediump float;\n"

    "void main() {\n"
        "  gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
    "}\n";

auto gVertexShader2 =
    "attribute vec4 vPosition;\n"

    "void main() {\n"
      "  gl_Position = vPosition + vec4(1.0, 1.0, 1.0, 1.0);"
    "}\n";

auto gFragmentShader2 =
    "precision mediump float;\n"

    "void main() {\n"
        "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
    "}\n";


//TODO try to find a way to actually edit shaders

#include <vector>
std::vector<GLuint> programs;

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
    programs.push_back(shader);
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
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
/*
    GLint formats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
    if (formats == 0) {
        LOGE("no binary formats supported");
    }
    LOGE("binary format number: %d", formats);*/
    // Get the binary length
    GLint length = 0;
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, &length);

    // Retrieve the binary code
    std::vector<GLubyte> buffer(length);
    GLenum format = 0;
    glGetProgramBinaryOES(program, length, NULL, &format, buffer.data());
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
GLuint gProgram2;
GLuint gvPositionHandle;
GLuint gvPositionHandle2;
GLint  sloc;
GLint  vloc;
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
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    sloc = glGetUniformLocation(gProgram, "s");
    aloc = glGetUniformLocation(gProgram, "a");
    bloc = glGetUniformLocation(gProgram, "b");
    checkGlError("glGetUniformLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
            gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f
};


int frame = 0;

void renderFrame() {
    static float grey = 0.0;
/*    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    */
    if (frame == 0) {
        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        glUseProgram(gProgram);
        checkGlError("glUseProgram");
        glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        glUniform4f(sloc, 0.3, 0.3, 0.3, 0.3);
        glUniform1f(aloc, 0.0);
        glUniform1f(bloc, 0.9);
        checkGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(gvPositionHandle);
        checkGlError("glEnableVertexAttribArray");
        glDrawArrays(GL_TRIANGLES, 0, 3);
        checkGlError("glDrawArrays");
        glDeleteProgram(gProgram);
        checkGlError("glDeleteProgram");
        glDeleteShader(programs[0]);
        checkGlError("glDeleteShader");
        glDeleteShader(programs[1]);
        checkGlError("glDeleteShader");
        LOGE("deleted shader");
    } else if (frame == 1) {
        raise(SIGWINCH);
        gProgram2 = createProgram(gVertexShader2, gFragmentShader2);
        if (!gProgram2) {
            while (1) {}
        }
        gvPositionHandle2 = glGetAttribLocation(gProgram2, "vPosition");
        sloc = glGetUniformLocation(gProgram2, "s");
        vloc = glGetUniformLocation(gProgram2, "v");
        aloc = glGetUniformLocation(gProgram2, "a");
        bloc = glGetUniformLocation(gProgram2, "b");

        GLint cur = 0;
        char name[20];

        for (int i = 0; i < 10; i++) {
/*            std::snprintf(name, 15, "unifloats[%d]", i);
            LOGE("initializing %s\n", name);
            cur = glGetUniformLocation(gProgram2, name);
            checkGlError("glGetUniformLocation");
            glUniform1f(cur, 0.3);*/
        }

        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        glUseProgram(gProgram2);
        checkGlError("glUseProgram");
        glVertexAttribPointer(gvPositionHandle2, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        glUniform4f(sloc, 0.3, 0.3, 0.3, 0.3);
        glUniform4f(vloc, 0.3, 0.3, 0.3, 0.3);
        glUniform1f(aloc, 0.5);
        glUniform1f(bloc, 0.9);
        checkGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(gvPositionHandle2);
        checkGlError("glEnableVertexAttribArray");
        glDrawArrays(GL_TRIANGLES, 0, 3);
        checkGlError("glDrawArrays");
        LOGE("frame 2");
    } else if (frame == 2) {
        raise(SIGWINCH);
        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");

        glUseProgram(gProgram2);
        checkGlError("glUseProgram");
        glVertexAttribPointer(gvPositionHandle2, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        glUniform4f(sloc, 0.3, 0.3, 0.3, 0.3);
        glUniform1f(aloc, 0.9);
        glUniform1f(bloc, 0.9);
        checkGlError("glVertexAttribPointer");
        glEnableVertexAttribArray(gvPositionHandle2);
        checkGlError("glEnableVertexAttribArray");
        glDrawArrays(GL_TRIANGLES, 0, 3);
        checkGlError("glDrawArrays");
        LOGE("frame 3");
        while (1) { sleep(1); }
    }
    frame++;
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
