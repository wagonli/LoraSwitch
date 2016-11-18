package com.orange.troisiemelieu.api;

import static java.lang.String.format;
import java.io.IOException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;
import com.orange.troisiemelieu.resource.SwitchResource;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;

@Component
public class SwitchResourceImpl implements SwitchResource {

    private final Logger log = LoggerFactory.getLogger(this.getClass());

    private final String commandUri;
    private final String apiKey;
    OkHttpClient client = new OkHttpClient();

    private static final MediaType JSON
            = MediaType.parse("application/json; charset=utf-8");

    public SwitchResourceImpl(@Value("${lom.uri}") String lomUri,
                              @Value("${device.uid}") String deviceUid,
                              @Value("${lom.api.key}") String apiKey) {
        this.apiKey = apiKey;
        commandUri = format("%s/api/v0/vendors/lora/devices/%s/commands", lomUri, deviceUid);
    }

    @Override
    public String switchPower(boolean switchValue) throws IOException {
        log.info("Received switch request with value {}", switchValue);
        RequestBody body = RequestBody.create(JSON, format("{\"data\": \"%s\", \"port\": 5, \"confirmed\": true}", switchValue ? "F1" : "F0"));
        Request request = new Request.Builder().url(commandUri)
                                               .header("X-API-KEY", apiKey)
                                               .header("Accept", "application/json")
                                               .post(body)
                                               .build();
        okhttp3.Response response = client.newCall(request).execute();
        return response.body().string();
    }
}
