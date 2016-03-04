package com.example.nativeopengl;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.Menu;

public class DemoActivity extends Activity 
{
	private GLSurfaceView view;

	@Override
	public void onCreate(Bundle savedInstanceState) 
{
		super.onCreate(savedInstanceState);

		view = new GLSurfaceView(this);

		// Tell EGL to use a ES 2.0 Context
		view.setEGLContextClientVersion(2);

		// Set the renderer
		view.setRenderer(new DemoProjectRenderer());

		setContentView(view);
	}

	@Override
	protected void onPause() 
{
		super.onPause();
		view.onPause();
	}

	@Override
	protected void onResume() 
{
		super.onResume();
		view.onResume();
	}
}