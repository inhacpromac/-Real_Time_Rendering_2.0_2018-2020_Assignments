package com.inhacpromac.win_view_as_object;

//default given packages
//for AppCompactActivity
import androidx.appcompat.app.AppCompatActivity;
//for Bundle
import android.os.Bundle;

//added by inhacpromac
//for Window class
import android.view.Window;
//for Window Manager
import android.view.WindowManager;
//for activity info
import android.content.pm.ActivityInfo;
//for Color
import android.graphics.Color;
//for View
import android.view.View;

//added by inhacpromac for view
//for AppCompatTextView
import androidx.appcompat.widget.AppCompatTextView;
//for Gravity/Position of letters here
import android.view.Gravity;

//AppCompactActivity
public class MainActivity extends AppCompatActivity {

    //Bundle
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		
        super.onCreate(savedInstanceState);
        //layout from Activity_Main.xml
        //setContentView(R.layout.activity_main);

        //this will get-rid of title bar
        this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
		//this will get-rid of notification bar
		this.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        //this will make Fullscreen
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //this will get forced Landscape Orientation
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        //this will set Background Color
        this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		
        //defining our own View
		AppCompatTextView myView  = new AppCompatTextView(this);
		myView.setTextColor(Color.rgb(0,255,0));
        myView.setTextSize(60);
        myView.setGravity(Gravity.CENTER);
        myView.setText("Hello World!!!");
        //set this view as our main view
        setContentView(myView);
		
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }
	
	

}
