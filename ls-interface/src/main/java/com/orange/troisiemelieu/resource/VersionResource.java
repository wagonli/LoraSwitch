package com.orange.troisiemelieu.resource;

import static javax.ws.rs.core.MediaType.TEXT_PLAIN;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;

@Path("/version")
public interface VersionResource {
    @GET
    @Produces(TEXT_PLAIN)
    String version();
}
