package com.example.nativeopengl;

class LibraryClass {
	static {
		System.loadLibrary("demo");
	}

	public static native void init();

	public static native void resize(int width, int height);

	public static native void render();
}