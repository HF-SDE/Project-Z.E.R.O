import mqtt from 'mqtt';

import config from '@config';

const client = mqtt.connect(config.MQTT_BROKER);

client.on('connect', () => {
  console.log('Connected');
});
