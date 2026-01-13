import mqtt from 'mqtt';

import config from '@config';

const client = mqtt.connect(config.MQTT_BROKER);

client.on('connect', () => console.log('Connected'));
client.on('disconnect', () => console.log('Disconnected'));

client.subscribe('clients/+/+/value').on('message', (topic, message) => {
  console.log(`Topic: ${topic} Message: ${message.toString()}`);
});
