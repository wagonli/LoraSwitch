package com.orange.troisiemelieu.configuration;

import static com.orange.troisiemelieu.tool.Exceptions.uncheck;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.io.Resource;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import com.orange.troisiemelieu.application.ManifestVersionReader;

@Configuration
@ComponentScan("com.orange.troisiemelieu.api")
public class SpringConfiguration {

    private static final Logger logger = LoggerFactory.getLogger(SpringConfiguration.class);

    @Bean
    public MessageChannel mqttInputChannel() {
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound(MqttPahoClientFactory clientFactory) {
        try {
            MqttPahoMessageDrivenChannelAdapter adapter =
                    new MqttPahoMessageDrivenChannelAdapter("roberto", clientFactory,
                                                            "router/~event/v1/data/new/urn/lora/70B3D59BA0000384/uplink");
            adapter.setCompletionTimeout(5000);
            adapter.setConverter(new DefaultPahoMessageConverter());
            adapter.setQos(1);
            adapter.setOutputChannel(mqttInputChannel());
            return adapter;
        } catch (Exception ex) {
            System.out.println(ex.toString());
            return null;
        }
    }

    @Bean
    @ServiceActivator(inputChannel = "mqttInputChannel")
    public MessageHandler handler() {
        return message -> logger.info(message.getPayload().toString());
    }

    @Bean
    public MqttPahoClientFactory mqttClientFactory(@Value("${lom.mqtt.uri}") String mqttUri,
                                                   @Value("${lom.api.key}") String lomApiKey) {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        factory.setServerURIs(mqttUri);
        factory.setUserName("payload");
        factory.setPassword(lomApiKey);
        return factory;
    }

    @Bean
    public ManifestVersionReader manifestVersionReader(ApplicationContext applicationContext) {
        Resource manifest = applicationContext.getResource("classpath:/META-INF/MANIFEST.MF");
        return new ManifestVersionReader(() -> uncheck(manifest::getInputStream));
    }
}
