#include <jni.h>
#include "jniExports.h"
#include "nativelibrary.h"

JNIEXPORT void JNICALL Java_com_example_nativeopengl_LibraryClass_init
(JNIEnv *env, jclass clazz)
{
	initializeOpenGL();
}

JNIEXPORT void JNICALL Java_com_example_nativeopengl_LibraryClass_resize
(JNIEnv *env, jclass clazz, jint width, jint height)
{
	resizeViewport(width, height);
}

JNIEXPORT void JNICALL Java_com_example_nativeopengl_LibraryClass_render(JNIEnv *env, jclass clazz)
{
	renderFrame();
}
