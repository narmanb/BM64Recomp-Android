import com.narmanb.bm64.touch.*;
public class TouchHarness {
    static void check(boolean value, String name) { if(!value)throw new AssertionError(name); }
    public static void main(String[] args) {
        TouchLayout layout=TouchLayout.defaults();
        TouchControl[] buttons={TouchControl.A,TouchControl.B,TouchControl.Z,TouchControl.R,TouchControl.L,TouchControl.START,
            TouchControl.C_UP,TouchControl.C_DOWN,TouchControl.C_LEFT,TouchControl.C_RIGHT,
            TouchControl.D_UP,TouchControl.D_DOWN,TouchControl.D_LEFT,TouchControl.D_RIGHT};
        int[] masks={0x8000,0x4000,0x2000,0x10,0x20,0x1000,8,4,2,1,0x800,0x400,0x200,0x100};
        for(int i=0;i<buttons.length;i++)check((1<<buttons[i].buttonBit)==masks[i],"N64 mapping "+buttons[i]);
        check(Math.abs(layout.opacity-.6f)<.001,"60% default");
        layout.visibility=TouchLayout.Visibility.ALWAYS;layout.opacity=.1f;
        layout.stickDeadzone=.12f;layout.stickSensitivity=72;
        layout.placement(TouchControl.A).x=.75f;
        TouchLayout read=TouchLayout.deserialize(layout.serialize());
        check(read.visibility==layout.visibility && read.opacity==.1f,"visibility/opacity persistence");
        check(read.stickSensitivity==72 && read.stickDeadzone==.12f,"response persistence");
        check(read.placement(TouchControl.A).x==.75f,"layout persistence");
        check(TouchVisibility.shown(TouchLayout.Visibility.ALWAYS,true,false,false),"always with gamepad");
        check(!TouchVisibility.shown(TouchLayout.Visibility.AUTO,true,false,false),"auto connected");
        check(TouchVisibility.shown(TouchLayout.Visibility.AUTO,false,false,false),"auto disconnected");
        check(!TouchVisibility.shown(TouchLayout.Visibility.NEVER,false,false,false),"off");
        check(TouchVisibility.shown(TouchLayout.Visibility.NEVER,true,false,true),"editor recovery");
        for(float opacity:new float[]{.1f,.6f,1f}) {
            layout.opacity=opacity;
            TouchPad pad=new TouchPad(layout);pad.setSurface(1920,1080,0,0,0,0);
            TouchLayout.Geometry stick=pad.geometryFor(TouchControl.STICK),a=pad.geometryFor(TouchControl.A),b=pad.geometryFor(TouchControl.B);
            check(pad.pointerDown(3,stick.cx+stick.rx/2,stick.cy),"stick hit");
            check(pad.pointerDown(8,a.cx,a.cy),"A hit");pad.pointerDown(12,b.cx,b.cy);
            check((pad.buttonMask()&0xc000)==0xc000,"A+B simultaneous independent of opacity");
            check(pad.stickX()>0&&pad.stickX()<.5&&pad.stickY()==0,"partial analog");
            pad.pointerDown(15,a.cx,a.cy);pad.pointerUp(8);
            check((pad.buttonMask()&0x8000)!=0,"second pointer retains button");
            pad.pointerUp(15);check((pad.buttonMask()&0x8000)==0,"A releases");
            pad.clear();check(pad.buttonMask()==0&&pad.stickX()==0,"cancel releases all");
        }
        TouchLayout corrupt=TouchLayout.deserialize("vis=nonsense;op=NaN;STICK=Infinity,-2,99,1");
        check(Float.isFinite(corrupt.opacity),"bad opacity");
        check(corrupt.placement(TouchControl.STICK).x<=1,"clamp layout");
        for(TouchControl c:TouchControl.values()){
            TouchLayout.Geometry g=layout.geometry(c,2400,1080,40,0,0,30);
            check(g.cx>=40&&g.cx<=2400&&g.cy<=1050,"safe area");
        }
        System.out.println("TouchHarness passed");
    }
}
