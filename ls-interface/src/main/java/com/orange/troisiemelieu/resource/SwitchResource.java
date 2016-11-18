package com.orange.troisiemelieu.resource;

import java.io.IOException;
import javax.ws.rs.PUT;
import javax.ws.rs.Path;
import javax.ws.rs.PathParam;

@Path("/switch")
public interface SwitchResource {
    @PUT
    @Path("/{switchValue}")
    String switchPower(@PathParam("switchValue") boolean switchValue) throws IOException;
}
