import com.narmanb.bm64.RomImporter;
import java.io.*;
import java.nio.file.*;
import java.util.Arrays;
import java.util.zip.*;

public class RomImporterHarness {
    static void check(boolean b){if(!b)throw new AssertionError();}
    static void reject(byte[] bytes, File destination) throws Exception {
        byte[] old=Files.readAllBytes(destination.toPath());
        try{RomImporter.importRom(new ByteArrayInputStream(bytes),destination);throw new AssertionError("accepted invalid ROM");}
        catch(IOException expected){}
        check(Arrays.equals(old,Files.readAllBytes(destination.toPath())));
        check(!new File(destination+".tmp").exists());
    }
    public static void main(String[] args) throws Exception {
        File dest=Files.createTempDirectory("bm64-import").resolve("bm64_us.z64").toFile();
        Files.write(dest.toPath(),new byte[]{1,2,3});
        reject(new byte[0],dest);reject(new byte[100],dest);
        byte[] fake=new byte[8*1024*1024];fake[0]=(byte)0x80;fake[1]=0x37;fake[2]=0x12;fake[3]=0x40;
        reject(fake,dest);reject(new byte[8*1024*1024+1],dest);
        ByteArrayOutputStream buf=new ByteArrayOutputStream();try(ZipOutputStream z=new ZipOutputStream(buf)){
            z.putNextEntry(new ZipEntry("rom.z64"));z.write(fake);z.closeEntry();
        }reject(buf.toByteArray(),dest);
        if(args.length>0){
            byte[] real=Files.readAllBytes(Paths.get(args[0]));
            RomImporter.importRom(new ByteArrayInputStream(real),dest);
            byte[] normalized=Files.readAllBytes(dest.toPath());
            for(int group:new int[]{2,4}){
                byte[] swapped=new byte[normalized.length];for(int i=0;i<swapped.length;i++)swapped[i]=normalized[i/group*group+group-1-i%group];
                RomImporter.importRom(new ByteArrayInputStream(swapped),dest);check(Arrays.equals(normalized,Files.readAllBytes(dest.toPath())));
            }
            buf.reset();try(ZipOutputStream z=new ZipOutputStream(buf)){z.putNextEntry(new ZipEntry("user.z64"));z.write(normalized);z.closeEntry();}
            RomImporter.importRom(new ByteArrayInputStream(buf.toByteArray()),dest);check(Arrays.equals(normalized,Files.readAllBytes(dest.toPath())));
        }
        dest.delete();dest.getParentFile().delete();System.out.println("RomImporterHarness passed"+(args.length==0?" (real ROM acceptance not tested)":""));
    }
}
