package com.narmanb.bm64;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.zip.*;

public final class DiagnosticFilesHarness {
    static void check(boolean value, String name) {if (!value) throw new AssertionError(name);}
    public static void main(String[] args) throws Exception {
        Path dir = Files.createTempDirectory("bm64-report");
        try {
            String first = "[RT64 startup] FIRST ERROR: GPU work rejected\n";
            String last = "LAST ERROR\n";
            String log = first + "vkQueueSubmit failed with error code 0xFFFFFFFC.\n".repeat(10000) + last;
            byte[] raw = log.getBytes(StandardCharsets.UTF_8);
            Files.write(dir.resolve("native-startup.log"), raw);
            Files.writeString(dir.resolve("startup-stage.txt"), "First UI frame recorded\n");
            Files.writeString(dir.resolve("graphics.json"), "{\"api_option\":\"Vulkan\"}");
            Files.writeString(dir.resolve("bm64_us.z64"), "PRIVATE ROM MUST NEVER BE EXPORTED");
            DiagnosticFiles diagnostics = new DiagnosticFiles(dir.toFile());
            String report = diagnostics.report();
            check(report.contains(first), "Copy report must retain startup/first error when repeated failures fill the log");
            check(report.contains(last), "Copy report must retain latest error");
            check(report.contains("omitted"), "Copy report must explicitly identify omitted bytes");
            check(report.length() < 70000, "Copy report remains bounded");
            ByteArrayOutputStream bytes = new ByteArrayOutputStream();
            try (ZipOutputStream zip = new ZipOutputStream(bytes)) {
                try {
                    DiagnosticFiles.class.getDeclaredMethod("appendToArchive", ZipOutputStream.class).invoke(diagnostics, zip);
                } catch (NoSuchMethodException e) {
                    throw new AssertionError("ZIP must include complete raw logs, not just bounded copy text", e);
                }
            }
            Map<String,byte[]> entries = new HashMap<>();
            try (ZipInputStream zip = new ZipInputStream(new ByteArrayInputStream(bytes.toByteArray()))) {
                ZipEntry entry;
                while ((entry = zip.getNextEntry()) != null) entries.put(entry.getName(), zip.readAllBytes());
            }
            check(Arrays.equals(raw, entries.get("diagnostics/native-startup.log")), "ZIP raw native log must be byte-for-byte complete");
            check(entries.containsKey("diagnostics/startup-stage.txt"), "ZIP contains raw checkpoints");
            check(entries.containsKey("diagnostics/graphics.json"), "ZIP contains graphics configuration");
            check(entries.keySet().stream().noneMatch(n -> n.endsWith(".z64")), "ZIP excludes ROM");
            Files.delete(dir.resolve("native-startup.log"));
            Files.writeString(dir.resolve("java-failure.txt"), "small failure\n");
            check(diagnostics.report().contains("small failure\n"), "Small report stays intact with missing native log");
            System.out.println("DiagnosticFilesHarness passed");
        } finally {
            try (var files = Files.walk(dir)) {
                for (Path p : files.sorted(Comparator.reverseOrder()).toList()) Files.delete(p);
            }
        }
    }
}
