import mqtt from 'mqtt';

import config from '@config';

const client = mqtt.connect(config.MQTT_BROKER);

client.on('connect', () => console.log('MQTT: \tConnected'));
client.on('disconnect', () => console.log('MQTT: \tisconnected'));

const maxTemp = 23;

const alarmClientId = '000094A4E48E0D84';
const alarmActivateTopic = `clients/${alarmClientId}/triggers/alarm-trigger`;
const alarmDeactivateTopic = `clients/${alarmClientId}/triggers/clear-alarm-trigger`;

client
  .subscribe('clients/+/tempature/value')
  .on('message', async (topic, message) => {
    const value = parseFloat(message.toString());

    if (value > maxTemp) {
      client.publish(
        alarmActivateTopic,
        JSON.stringify({
          useSound: false,
          message: `High temperature detected: ${value}°C`,
        }),
      );
    } else {
      client.publish(alarmDeactivateTopic, '');
    }
  });
