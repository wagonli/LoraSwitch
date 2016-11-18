package com.orange.troisiemelieu.configuration.jaxrs;

import java.io.IOException;
import java.time.Instant;
import java.util.Date;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;

public class InstantUnixTimestampDeserializer extends JsonDeserializer<Instant> {
    public InstantUnixTimestampDeserializer() {
    }

    public Instant deserialize(JsonParser jp, DeserializationContext ctxt) throws IOException {
        String text = jp.getText().trim();
        if(text.isEmpty()) {
            return null;
        } else {
            try {
                return this.makeInstantFrom(Long.valueOf(text).longValue());
            } catch (IllegalArgumentException var5) {
                throw ctxt.weirdStringException(text, Date.class, "not a valid unix timestamp representation");
            }
        }
    }

    private Instant makeInstantFrom(long unixTimestamp) {
        return Instant.ofEpochSecond(unixTimestamp);
    }
}
