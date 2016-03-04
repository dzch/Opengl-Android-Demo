package com.example.nativeopengl;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;

class DemoProjectRenderer implements Renderer {

	public void onDrawFrame(GL10 gl) {
		LibraryClass.render();
	}

	public void onSurfaceChanged(GL10 gl, int width, int height) {
		LibraryClass.resize(width, height);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		LibraryClass.init();
	}
}