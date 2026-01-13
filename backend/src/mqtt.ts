import mqtt from 'mqtt';

import config from '@config';

const client = mqtt.connect(config.MQTT_BROKER);

client.on('connect', () => console.log('MQTT: \tConnected'));
client.on('disconnect', () => console.log('MQTT: \tisconnected'));
client.on('end', () => console.log('MQTT: \tended'));
client.on('error', (error) => console.error('MQTT: \tError', error));

const maxTemp = 23;

const alarmClientId = '000094A4E48E0D84';
const alarmActivateTopic = `clients/${alarmClientId}/triggers/alarm-trigger`;
const alarmDeactivateTopic = `clients/${alarmClientId}/triggers/clear-alarm-trigger`;

client
  .subscribe('clients/+/temperature/value')
  .on('message', async (topic, message) => {
    if (!topic.startsWith('clients/') && !topic.endsWith('/temperature/value'))
      return;

    const value = parseFloat(message.toString());

    if (value > maxTemp) {
      client.publish(
        alarmActivateTopic,
        JSON.stringify({
          useSound: false,
          message: `High temperature detected: ${value} C`,
        }),
      );
    } else {
      client.publish(alarmDeactivateTopic, '');
    }
  });
