package com.orange.troisiemelieu.configuration.jaxrs;

import static com.fasterxml.jackson.annotation.JsonAutoDetect.Visibility.NONE;
import static com.fasterxml.jackson.annotation.JsonAutoDetect.Visibility.PUBLIC_ONLY;
import static com.fasterxml.jackson.annotation.JsonCreator.Mode.PROPERTIES;
import static com.fasterxml.jackson.annotation.PropertyAccessor.CREATOR;
import static com.fasterxml.jackson.annotation.PropertyAccessor.GETTER;
import static com.fasterxml.jackson.annotation.PropertyAccessor.IS_GETTER;
import static com.fasterxml.jackson.annotation.PropertyAccessor.SETTER;
import java.time.Instant;
import javax.ws.rs.ext.ContextResolver;
import javax.ws.rs.ext.Provider;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.module.SimpleModule;
import com.fasterxml.jackson.module.paramnames.ParameterNamesModule;

@Provider
public class JacksonMapperProvider implements ContextResolver<ObjectMapper> {

    final ObjectMapper objectMapper;

    public JacksonMapperProvider() {
        objectMapper = createMapper();
    }

    public static ObjectMapper createMapper() {
        SimpleModule module = new SimpleModule("unix_timestamp_mapping_module");
        module.addSerializer(Instant.class, new InstantUnixTimestampSerializer());
        module.addDeserializer(Instant.class, new InstantUnixTimestampDeserializer());

        ObjectMapper mapper = new ObjectMapper();
        mapper.registerModule(module);
        mapper.registerModule(new ParameterNamesModule(PROPERTIES));
        mapper.setVisibility(CREATOR, PUBLIC_ONLY);
        mapper.setVisibility(GETTER, NONE);
        mapper.setVisibility(IS_GETTER, NONE);
        mapper.setVisibility(SETTER, NONE);
        return mapper;
    }

    @Override
    public ObjectMapper getContext(Class<?> type) {
        return objectMapper;
    }
}
