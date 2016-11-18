package com.orange.troisiemelieu;

import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.boot.web.support.SpringBootServletInitializer;

@SpringBootApplication
public class LoraswitchApplication extends SpringBootServletInitializer {

    public static void main(String[] args) {
        new LoraswitchApplication()
                .configure(new SpringApplicationBuilder(LoraswitchApplication.class))
                .run(args);
    }
}
