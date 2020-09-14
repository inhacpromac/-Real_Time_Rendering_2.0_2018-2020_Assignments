package com.inhacpromac.blue_window;

//added by inhacpromac
//for context
import android.content.Context;
//for such a view which will give OpenGL supported surface
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
//for touchevent
import android.view.MotionEvent;
//for gesture event
import android.view.GestureDetector;
//for gesture event's OnGestureListener
import android.view.GestureDetector.OnGestureListener;
//for gesture event's OnDoubleTapListener
import android.view.GestureDetector.OnDoubleTapListener;

//for basic feature of OpenGLES
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

//implements OnGestureListener, OnDoubleTapListener :means
//all events in this class are manditory to use even though it is empty
//implementing inner class of GLSurfaceView, Renderer
public class GlesView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
    private GestureDetector gestureDetector;

    public GlesView(Context drawingContext) {

        super(drawingContext);
		
		context = drawingContext;
		
		setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        gestureDetector = new GestureDetector(drawingContext, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        //this is most important line but not in use under openGL programming.
        //this line is for keyboard gesture.
        int eventAction = event.getAction();

        if (!gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        return true;
    }

    //OnDoubleTapListener
    @Override
    public boolean onDoubleTap(MotionEvent event) {
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
        return true;
    }

    //OnGestureListener
    @Override
    public boolean onDown(MotionEvent event) {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent event) {
		
    }

    @Override
    public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
        System.exit(0);
        return true;
    }

    @Override
    public void onShowPress(MotionEvent event) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return true;
    }
	
	//Implement GLSurfaceView.Renderer methods
	@Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
        System.out.println("INHACPROMAC: " + version);

        initialize();
    }
	//ususally don't use OpenGL fuctions is resemble by GL10 unused..
    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        display();
    }
	
	//Our custom methods
	private void initialize() {
        GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }
	
	private void resize(int width, int height) {
		if (height == 0)
		{
			height = 1;
		}
        GLES32.glViewport(0, 0, width, height);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        requestRender();
    }

}
