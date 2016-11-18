package com.orange.troisiemelieu.configuration;

import javax.servlet.ServletContext;
import javax.ws.rs.core.Context;
import org.glassfish.jersey.server.ResourceConfig;
import org.springframework.context.annotation.Configuration;
import com.orange.troisiemelieu.api.SwitchResourceImpl;
import com.orange.troisiemelieu.api.VersionResourceImpl;

@Configuration
public class JerseyConfiguration extends ResourceConfig {

    public JerseyConfiguration() {
        packages("com.orange.troisiemelieu.configuration.jaxrs");
        registerClasses(SwitchResourceImpl.class, VersionResourceImpl.class);

    }

    @Context
    public void setContext(ServletContext context) {
        /*WebApplicationContextUtils.getWebApplicationContext(context)
                                  .getBeansWithAnnotation(Path.class)
                                  .forEach((name, resource) -> register(resource));*/
    }
}
