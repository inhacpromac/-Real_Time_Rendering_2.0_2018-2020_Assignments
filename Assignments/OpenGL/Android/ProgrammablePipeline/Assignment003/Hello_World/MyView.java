package com.inhacpromac.win_hello_world;

//added by inhacpromac
//for AppCompatTextView
import androidx.appcompat.widget.AppCompatTextView;
//for context
import android.content.Context;
//for Color
import android.graphics.Color;
//for Gravity/Position of letters here
import android.view.Gravity;

public class MyView extends AppCompatTextView {

    public MyView(Context drawingContext) {

        super(drawingContext);
        setTextColor(Color.rgb(0,255,0));
        setTextSize(60);
        setGravity(Gravity.CENTER);
        setText("Hello World!!!");
    }

}
