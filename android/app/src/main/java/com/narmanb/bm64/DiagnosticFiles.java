// Adapted from BMHeroRecomp-android (GPL-3.0); see android/NOTICE.md.
package com.narmanb.bm64;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

/** Files are shared with the game process and survive a native crash. */
final class DiagnosticFiles {
    private final File directory;
    DiagnosticFiles(File directory) { this.directory = directory; }
    void beginAttempt() {
        directory.mkdirs();
        for (String name : new String[]{"startup-stage.txt", "native-startup.log", "java-failure.txt", "graphics-failure.txt"})
            new File(directory, name).delete();
        stage("Starting game process");
    }
    void stage(String stage) { write("startup-stage.txt", System.currentTimeMillis() + " " + stage + "\n", true); }
    void failure(Throwable error) {
        StringWriter text = new StringWriter();
        error.printStackTrace(new PrintWriter(text));
        write("java-failure.txt", text.toString(), false);
    }
    private void write(String name, String text, boolean append) {
        try (FileOutputStream out = new FileOutputStream(new File(directory, name), append)) {
            out.write(text.getBytes(StandardCharsets.UTF_8));
            out.getFD().sync();
        } catch (IOException ignored) { /* Diagnostics must not cause a second failure. */ }
    }
    private static void appendBytes(StringBuilder out, RandomAccessFile in, int size) throws IOException {
        byte[] bytes = new byte[size];
        in.readFully(bytes);
        out.append(new String(bytes, StandardCharsets.UTF_8));
    }
    /** Stream complete allowlisted raw files; never traverse game/ROM storage. */
    void appendToArchive(ZipOutputStream zip) throws IOException {
        for (String name : new String[]{"startup-stage.txt", "java-failure.txt", "graphics-failure.txt", "native-startup.log", "graphics.json", "rt64/rt64.log"}) {
            File file = new File(directory, name);
            if (!file.isFile()) continue;
            try (RandomAccessFile in = new RandomAccessFile(file, "r")) {
                // Capture a finite size even if the live game is still appending errors.
                long remaining = in.length();
                zip.putNextEntry(new ZipEntry("diagnostics/" + name));
                byte[] buffer = new byte[65536];
                while (remaining > 0) {
                    int n = in.read(buffer, 0, (int)Math.min(buffer.length, remaining));
                    if (n < 0) throw new EOFException("Diagnostic file changed during export: " + name);
                    zip.write(buffer, 0, n);
                    remaining -= n;
                }
                zip.closeEntry();
            }
        }
    }
    String report() {
        StringBuilder out = new StringBuilder();
        for (String name : new String[]{"startup-stage.txt", "java-failure.txt", "graphics-failure.txt", "native-startup.log"}) {
            File file = new File(directory, name);
            if (!file.isFile()) continue;
            out.append("\n--- ").append(name).append(" ---\n");
            try (RandomAccessFile in = new RandomAccessFile(file, "r")) {
                long length = in.length();
                int limit = name.equals("native-startup.log") ? 65536 : 8192;
                if (length <= limit) {
                    appendBytes(out, in, (int)length);
                } else {
                    // Preserve initialization and the first failure as well as the latest
                    // errors. A tail-only preview loses the cause under an error storm.
                    int prefix = limit * 3 / 4, suffix = limit - prefix;
                    appendBytes(out, in, prefix);
                    out.append("\n[... ").append(length - limit)
                        .append(" bytes omitted from copy preview; full file is in ZIP diagnostics/")
                        .append(name).append(" ...]\n");
                    in.seek(length - suffix);
                    appendBytes(out, in, suffix);
                }
            } catch (IOException e) { out.append("Cannot read: ").append(e); }
        }
        return out.toString();
    }
}
