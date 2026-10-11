package com.narmanb.bm64.touch;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;

/** Fresh reads and atomic replacement work across the launcher and :game processes. */
public final class TouchStore {
    private TouchStore() {}
    public static TouchLayout load(File file) {
        try {
            if (file.length() > 65536) return TouchLayout.defaults();
            return TouchLayout.deserialize(new String(Files.readAllBytes(file.toPath()),StandardCharsets.UTF_8));
        } catch(IOException | RuntimeException e) { return TouchLayout.defaults(); }
    }
    public static void save(File file, TouchLayout layout) throws IOException {
        File parent=file.getParentFile();
        if (!parent.isDirectory() && !parent.mkdirs()) throw new IOException("Cannot create touch settings directory");
        File temp=File.createTempFile("touch-", ".tmp", parent);
        try {
            try(FileOutputStream out=new FileOutputStream(temp)) {
                out.write(layout.serialize().getBytes(StandardCharsets.UTF_8));out.getFD().sync();
            }
            Files.move(temp.toPath(),file.toPath(),StandardCopyOption.REPLACE_EXISTING,StandardCopyOption.ATOMIC_MOVE);
        } finally {temp.delete();}
    }
}
