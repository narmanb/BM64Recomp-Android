package com.narmanb.bm64;

import android.app.*;
import android.content.*;
import android.view.*;
import android.widget.*;
import org.json.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;

/** Launcher settings use native JSON formats; touchscreen settings stay in TouchPrefs. */
final class AndroidSettings {
    private AndroidSettings() {}
    static void touch(Activity a) {
        a.startActivity(new Intent(a,ControlsActivity.class).putExtra("settings",true));
    }
    static int sensitivity(Activity a) {return Math.max(0,Math.min(100,read(a,"controller_response.json").optInt("sensitivity",100)));}
    static File directory(Activity a) { return new File(a.getFilesDir(),"bm64"); }
    static JSONObject read(Activity a,String name) {
        try { return new JSONObject(new String(Files.readAllBytes(new File(directory(a),name).toPath()),StandardCharsets.UTF_8)); }
        catch(Exception e) { return new JSONObject(); }
    }
    static boolean gameRunning(Activity a) {
        ActivityManager am=(ActivityManager)a.getSystemService(Context.ACTIVITY_SERVICE);
        java.util.List<ActivityManager.RunningAppProcessInfo> list=am.getRunningAppProcesses();
        if(list!=null)for(ActivityManager.RunningAppProcessInfo p:list)if(p.processName.equals(a.getPackageName()+":game"))return true;
        return false;
    }
    static void save(Activity a,String name,JSONObject json) throws IOException {
        File dest=new File(directory(a),name),temp=new File(directory(a),name+".tmp");directory(a).mkdirs();
        try {
            try(FileOutputStream out=new FileOutputStream(temp)) {out.write(json.toString(2).getBytes(StandardCharsets.UTF_8));out.getFD().sync();}
            Files.move(temp.toPath(),dest.toPath(),StandardCopyOption.REPLACE_EXISTING,StandardCopyOption.ATOMIC_MOVE);
        } catch(JSONException e) {throw new IOException(e);} finally {temp.delete();}
    }
    private static boolean editable(Activity a) {
        if(!gameRunning(a))return true;
        new AlertDialog.Builder(a).setMessage("Use the in-game settings while a game is running. Return to the launcher using Android Back to close the game before changing launch defaults.")
            .setPositiveButton("OK",null).show();return false;
    }
    private static LinearLayout column(Activity a) {LinearLayout l=new LinearLayout(a);l.setOrientation(1);l.setPadding(24,8,24,8);return l;}
    private static void text(Activity a,LinearLayout l,String label) {TextView t=new TextView(a);t.setText(label);l.addView(t);}
    private static Spinner options(Activity a,LinearLayout l,String label,String[] choices,int selected) {
        text(a,l,label);Spinner s=new Spinner(a);s.setAdapter(new ArrayAdapter<>(a,android.R.layout.simple_spinner_dropdown_item,choices));
        s.setSelection(selected);l.addView(s);return s;
    }
    static void graphics(Activity a) {
        if(!editable(a))return;
        JSONObject cfg=read(a,"graphics.json");LinearLayout l=column(a);
        text(a,l,"Rendering FPS does not change game timing.");
        Spinner aspect=options(a,l,"Aspect ratio",new String[]{"Original 4:3","Widescreen"},cfg.optString("ar_option","Expand").equals("Original")?0:1);
        String[] res={"Original","Original2x","Original3x","Original4x","Original6x","Auto"};
        Spinner resolution=options(a,l,"Internal resolution",new String[]{"1x — about 240p","2x — about 480p","3x — about 720p","4x — about 960p","6x — about 1440p","Match screen"},index(res,cfg.optString("res_option","Original2x")));
        text(a,l,"Higher resolution sharpens 3D edges and detail but costs GPU time and memory. Start with 3x or 4x; 6x is an optional high quality setting. Original textures keep their original detail.");
        int[] downsampleValues={1,2,4};
        int downsample=cfg.optInt("ds_option",1);
        Spinner supersampling=options(a,l,"Supersampling / downsampling",new String[]{"Off","2x — smoother edges","4x — highest GPU cost"},downsample==4?2:downsample==2?1:0);
        text(a,l,"Renders at 2x or 4x the selected resolution in each direction, then reduces the image. Only applies to 1x or 2x internal resolution; higher scales and Match screen ignore the saved supersampling setting.");
        resolution.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            public void onItemSelected(AdapterView<?> parent,View view,int position,long id) {supersampling.setEnabled(position<2);}
            public void onNothingSelected(AdapterView<?> parent) {}
        });
        supersampling.setEnabled(resolution.getSelectedItemPosition()<2);
        String[] msaa={"None","MSAA2X","MSAA4X","MSAA8X"};
        Spinner antialiasing=options(a,l,"MS anti-aliasing",new String[]{"Off","2x","4x","8x — highest GPU cost"},index(msaa,cfg.optString("msaa_option","None")));
        text(a,l,"Smooths polygon edges. GPU support is checked when starting the game; unsupported modes fall back. Supersampling also smooths textures and can be used without MSAA.");
        String[] precision={"Auto","Off","On"};
        Spinner color=options(a,l,"Framebuffer color precision",new String[]{"Automatic (recommended)","Standard","High — may reduce color banding"},index(precision,cfg.optString("hpfb_option","Auto")));
        text(a,l,"High precision uses more GPU memory and bandwidth, with a fallback to Standard if the GPU lacks support. This changes internal color calculations, not display HDR. Takes effect on the next game launch.");
        String[] hud={"Original","Clamp16x9","Full"};
        Spinner placement=options(a,l,"HUD placement",new String[]{"Original 4:3","16:9","Expand to screen edges"},index(hud,cfg.optString("hr_option","Clamp16x9")));
        String[] fps={"Original","Manual","Display"};
        Spinner rate=options(a,l,"Rendering frame rate",new String[]{"Original","60 FPS target","Display refresh"},index(fps,cfg.optString("rr_option","Manual")));
        ScrollView scroll=new ScrollView(a);scroll.addView(l);
        new AlertDialog.Builder(a).setTitle("Graphics Settings").setView(scroll).setNegativeButton("Cancel",null).setPositiveButton("Save",(d,w)-> {
            try {cfg.put("ar_option",aspect.getSelectedItemPosition()==0?"Original":"Expand");cfg.put("res_option",res[resolution.getSelectedItemPosition()]);
                cfg.put("ds_option",downsampleValues[supersampling.getSelectedItemPosition()]);cfg.put("msaa_option",msaa[antialiasing.getSelectedItemPosition()]);
                cfg.put("hpfb_option",precision[color.getSelectedItemPosition()]);cfg.put("hr_option",hud[placement.getSelectedItemPosition()]);
                cfg.put("rr_option",fps[rate.getSelectedItemPosition()]);cfg.put("rr_manual_value",60);cfg.put("api_option","Vulkan");
                cfg.put("wm_option","Fullscreen");cfg.put("developer_mode",false);save(a,"graphics.json",cfg);
            }catch(Exception e){error(a,e);}
        }).show();
    }
    private static int index(String[] a,String s) {for(int i=0;i<a.length;i++)if(a[i].equals(s))return i;return 0;}
    static void controllers(Activity a) {
        if(!editable(a))return;
        LinearLayout l=column(a);ScrollView scroll=new ScrollView(a);scroll.addView(l);
        text(a,l,"SDL automatically detects integrated, USB and Bluetooth gamepads. Default: left stick moves, South places bomb, West lifts/talks, triggers detonate, RB stops bomb, right stick controls camera, D-pad navigates, Start pauses, Select opens settings. Full remapping is also available in-game.");
        JSONObject controls=read(a,"controls.json"),general=read(a,"general.json");
        JSONObject existing=controls.optJSONObject("controller");final JSONObject mappings=existing==null?new JSONObject():existing;
        String[] names={"A","B","Z","R","L","START","C_UP","C_DOWN","C_LEFT","C_RIGHT","DPAD_UP","DPAD_DOWN","DPAD_LEFT","DPAD_RIGHT","TOGGLE_MENU"};
        String[] labels={"A — Place bomb","B — Lift/Talk","Z — Detonate","R — Stop bomb","L","Start / Pause","C Up — Zoom in","C Down — Zoom out","C Left — Camera left","C Right — Camera right","D-pad Up","D-pad Down","D-pad Left","D-pad Right","Settings"};
        String[] choices={"Keep existing","South (A)","East (B)","West (X)","North (Y)","Select","Start","LB","RB","Left trigger","Right trigger","Right stick up","Right stick down","Right stick left","Right stick right","D-pad up","D-pad down","D-pad left","D-pad right","Unbound"};
        int[][] fields={{0,0},{3,0},{3,1},{3,2},{3,3},{3,4},{3,6},{3,9},{3,10},{4,5},{4,6},{4,-4},{4,4},{4,-3},{4,3},{3,11},{3,12},{3,13},{3,14},{0,0}};
        // Menu handling accepts digital, non-D-pad buttons; axes and D-pad are gameplay-only.
        int[] menuChoices={0,1,2,3,4,5,6,7,8,19};
        String[] menuLabels=new String[menuChoices.length];for(int i=0;i<menuLabels.length;i++)menuLabels[i]=choices[menuChoices[i]];
        Spinner[] rows=new Spinner[names.length];for(int i=0;i<rows.length;i++)rows[i]=options(a,l,labels[i],i==rows.length-1?menuLabels:choices,0);
        text(a,l,"Left stick deadzone (0–50%)");SeekBar dead=new SeekBar(a);dead.setMax(50);dead.setProgress(general.optInt("joystick_deadzone",0));l.addView(dead);
        text(a,l,"Left stick sensitivity (0 = fine movement, 100 = linear)");SeekBar sensitivity=new SeekBar(a);sensitivity.setMax(100);
        sensitivity.setProgress(sensitivity(a));l.addView(sensitivity);
        new AlertDialog.Builder(a).setTitle("Physical Controller Settings").setView(scroll).setNegativeButton("Cancel",null).setPositiveButton("Save",(d,w)-> {
            try {for(int i=0;i<names.length;i++) {int selected=rows[i].getSelectedItemPosition();if(selected==0)continue;
                int[] field=fields[i==names.length-1?menuChoices[selected]:selected];JSONArray bindings=new JSONArray();bindings.put(new JSONObject().put("input_type",field[0]).put("input_id",field[1]));
                // Explicitly clear secondary bindings when remapping or unbinding.
                bindings.put(new JSONObject().put("input_type",0).put("input_id",0));mappings.put(names[i],bindings);
            }
            controls.put("controller",mappings);save(a,"controls.json",controls);general.put("joystick_deadzone",dead.getProgress());save(a,"general.json",general);
            save(a,"controller_response.json",new JSONObject().put("sensitivity",sensitivity.getProgress()));
            }catch(Exception e){error(a,e);}
        }).show();
    }
    static void error(Activity a,Exception e) {new AlertDialog.Builder(a).setMessage(e.getMessage()).setPositiveButton("OK",null).show();}
}
