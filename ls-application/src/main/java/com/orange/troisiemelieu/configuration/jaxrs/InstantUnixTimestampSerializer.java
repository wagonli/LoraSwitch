package com.orange.troisiemelieu.configuration.jaxrs;

import java.io.IOException;
import java.time.Instant;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;

public class InstantUnixTimestampSerializer extends JsonSerializer<Instant> {
    public InstantUnixTimestampSerializer() {
    }

    public void serialize(Instant date, JsonGenerator jgen, SerializerProvider serializerProvider) throws IOException {
        jgen.writeNumber(date.getEpochSecond());
    }
}
