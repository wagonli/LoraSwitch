package com.orange.troisiemelieu.api;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import com.orange.troisiemelieu.application.ManifestVersionReader;
import com.orange.troisiemelieu.resource.VersionResource;

@Component
public class VersionResourceImpl implements VersionResource {

    private final ManifestVersionReader manifestVersionReader;

    @Autowired
    public VersionResourceImpl(ManifestVersionReader manifestVersionReader) {
        this.manifestVersionReader = manifestVersionReader;
    }

    @Override
    public String version() {
        return "1.0.0";
    }
}
