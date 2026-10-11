import com.narmanb.bm64.touch.*;
import java.io.*;
import java.nio.file.*;

public class TouchStoreHarness {
    static void check(boolean value,String name) {if(!value)throw new AssertionError(name);}
    public static void main(String[] args) throws Exception {
        File dir=Files.createTempDirectory("bm64-settings").toFile();File file=new File(dir,"touch.txt");
        try {
            TouchLayout launcher=TouchStore.load(file);check(launcher.opacity==.6f,"missing defaults");
            launcher.opacity=.3f;launcher.visibility=TouchLayout.Visibility.ALWAYS;TouchStore.save(file,launcher);
            TouchLayout game=TouchStore.load(file);check(game.opacity==.3f,"game sees launcher save");
            game.opacity=.75f;game.visibility=TouchLayout.Visibility.NEVER;game.placement(TouchControl.C_UP).x=.42f;
            TouchStore.save(file,game);
            // A launcher process already read this file before the other process changed it.
            launcher=TouchStore.load(file);
            check(launcher.opacity==.75f&&launcher.visibility==TouchLayout.Visibility.NEVER,"fresh cross-process read");
            check(launcher.placement(TouchControl.C_UP).x==.42f,"layout not overwritten by stale cache");
            launcher.visibility=TouchLayout.Visibility.AUTO;TouchStore.save(file,launcher);
            check(TouchStore.load(file).opacity==.75f,"changing mode retains saved opacity");
            check(dir.list().length==1,"no temporary files");
            Files.writeString(file.toPath(),"op=NaN;vis=invalid");
            check(Float.isFinite(TouchStore.load(file).opacity),"corrupt file defaults");
            System.out.println("TouchStoreHarness passed");
        } finally {file.delete();dir.delete();}
    }
}
