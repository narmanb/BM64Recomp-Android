package com.narmanb.bm64;
import android.app.Activity;
import android.os.*;
import android.view.*;
import android.widget.*;
import com.narmanb.bm64.touch.*;
import java.util.Locale;

/** ROM-free control preview; never initializes or simulates the game. */
public final class ControlsActivity extends Activity {
    private TouchOverlayController overlay;
    private TextView state;
    private final Handler handler=new Handler(Looper.getMainLooper());
    private boolean nativeReady;
    private String physical="Connect a gamepad to inspect Android input events.";
    private final Runnable poll=new Runnable(){public void run(){
        if(nativeReady) {
            float[] v=NativeTouch.nativePollPreview();
            state.setText(String.format(Locale.US,"Control preview — no game is running\nTouch N64 buttons: %04X  Stick: %.2f, %.2f\n%s",(int)v[0],v[1],v[2],physical));
        }
        handler.postDelayed(this,50);
    }};
    @Override protected void onCreate(Bundle saved){
        super.onCreate(saved);
        try{System.loadLibrary("c++_shared");System.loadLibrary("BM64Platform");nativeReady=true;}catch(UnsatisfiedLinkError e){nativeReady=false;}
        FrameLayout frame=new FrameLayout(this);frame.setBackgroundColor(0xff152130);
        state=new TextView(this);state.setTextColor(0xffffffff);state.setGravity(Gravity.CENTER);state.setText("Layout preview — native input bridge unavailable");
        frame.addView(state,new FrameLayout.LayoutParams(-1,-1));setContentView(frame);
        overlay=new TouchOverlayController(this);overlay.attachTo(frame);
        LinearLayout toolbar=new LinearLayout(this);
        Button settings=new Button(this);settings.setText("Touch settings");settings.setOnClickListener(v->overlay.showSettings(frame));toolbar.addView(settings);
        Button edit=new Button(this);edit.setText("Edit layout");edit.setOnClickListener(v->overlay.startEditing(frame));toolbar.addView(edit);
        Button done=new Button(this);done.setText("Back to launcher");done.setOnClickListener(v->finish());toolbar.addView(done);
        FrameLayout.LayoutParams lp=new FrameLayout.LayoutParams(-2,-2,Gravity.TOP|Gravity.CENTER_HORIZONTAL);frame.addView(toolbar,lp);
        if(getIntent().getBooleanExtra("settings",false))frame.post(()->overlay.showSettings(frame));
    }
    @Override public boolean dispatchKeyEvent(KeyEvent e){
        if(TouchOverlayView.isGamepadEvent(e)) {
            physical="Gamepad "+e.getDeviceId()+": "+KeyEvent.keyCodeToString(e.getKeyCode())+" "+(e.getAction()==0?"down":"up");return true;
        }
        return super.dispatchKeyEvent(e);
    }
    @Override public boolean dispatchGenericMotionEvent(MotionEvent e){
        if(TouchOverlayView.isGamepadEvent(e)) {
            physical=String.format(Locale.US,"Gamepad %d: X %.2f Y %.2f / hat %.0f %.0f",e.getDeviceId(),e.getAxisValue(MotionEvent.AXIS_X),e.getAxisValue(MotionEvent.AXIS_Y),e.getAxisValue(MotionEvent.AXIS_HAT_X),e.getAxisValue(MotionEvent.AXIS_HAT_Y));return true;
        }
        return super.dispatchGenericMotionEvent(e);
    }
    @Override protected void onResume(){super.onResume();overlay.onResume();handler.post(poll);}
    @Override protected void onPause(){handler.removeCallbacks(poll);overlay.onPause();super.onPause();}
    @Override public void onWindowFocusChanged(boolean focus){super.onWindowFocusChanged(focus);if(!focus&&overlay!=null)overlay.view().release();}
}
