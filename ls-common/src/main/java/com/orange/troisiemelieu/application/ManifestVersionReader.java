package com.orange.troisiemelieu.application;

import static java.util.Objects.requireNonNull;
import java.io.IOException;
import java.io.InputStream;
import java.util.function.Supplier;
import java.util.jar.Manifest;

public class ManifestVersionReader {
    private Supplier<InputStream> inputStreamSupplier;

    public ManifestVersionReader(Supplier<InputStream> inputStreamSupplier) {
        this.inputStreamSupplier = requireNonNull(inputStreamSupplier);
    }

    public String readAppVersion() {
        try(InputStream inputStream = requireNonNull(inputStreamSupplier.get())) {
            Manifest manifest = new Manifest(inputStream);
            StringBuilder versionBuilder = new StringBuilder(manifest.getMainAttributes().getValue("Project-Version"))
                    .append(" [")
                    .append("built: ").append(manifest.getMainAttributes().getValue("Build-Date"))
                    .append(", sha: ").append(manifest.getMainAttributes().getValue("Git-CommitId"));

            String prId = manifest.getMainAttributes().getValue("GitHub-PullRequestId");
            if (prId != null) {
                versionBuilder.append(", pr: ").append(prId);
            }

            versionBuilder.append("]");
            return versionBuilder.toString();
        } catch (IOException e) {
            return "Error while trying to retrieve version from MANIFEST.MF. Cause: " + e.getMessage();
        }
    }
}
