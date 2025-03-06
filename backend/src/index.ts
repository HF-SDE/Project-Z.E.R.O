import bodyParser from 'body-parser';
import cors from 'cors';
import { rateLimit } from 'express-rate-limit';
import helmet from 'helmet';
import mqtt, { MqttClient } from 'mqtt';
import passport from 'passport';

import app from '@app';
import config from '@config';
import prisma from '@prisma-instance';
import alertRoutes from '@routes/alert.routes';
import authRoutes from '@routes/auth.routes';
import deviceRoutes from '@routes/device.routes';
import locationRoutes from '@routes/location.routes';
import manageRoutes from '@routes/manage.routes';
import profileRoutes from '@routes/profile.routes';
import stats from '@routes/stats.routes';
import timeSeriesRoutes from '@routes/timeSeries.routes';
import tsAlertsRoutes from '@routes/tsAlerts.routes';

import './passport';
import { MQTTData } from '@api-types/mqttclient.types';

const limiter = rateLimit({
  windowMs: config.RATE_LIMIT_RESET_SEC * 1000, // 60 minutes
  limit: config.RATE_LIMIT_COUNT, // Limit each IP to 100 requests per `window` (here, per 15 minutes).
  standardHeaders: 'draft-7', // draft-6: `RateLimit-*` headers; draft-7: combined `RateLimit` header
  legacyHeaders: false, // Disable the `X-RateLimit-*` headers.
});

app.set('trust proxy', 1);
app.set('json spaces', 4);

app.use(cors());

app.use(helmet());
app.use(bodyParser.json({}));
app.use(passport.initialize());
app.use(limiter);

app.use('/', authRoutes);
app.use('/manage', manageRoutes);
app.use('/profile', profileRoutes);
app.use('/device', deviceRoutes);
app.use('/location', locationRoutes);
app.use('/alert', alertRoutes);
app.use('/data', timeSeriesRoutes);
app.use('/tsalert', tsAlertsRoutes);
app.use('/stats', stats);

app.get('/health', (req, res) => {
  res.status(200).json({ status: 'ok' });
});

app.listen(config.PORT, () => {
  console.info(`Server is running on ${config.PORT}`);
});

// MQTT Connection
const client: MqttClient = mqtt.connect(config.MQTT_BROKER, {
  username: config.MQTT_USERNAME,
  password: config.MQTT_PASSWORD,
});

client.on('connect', () => {
  console.info('✅ Connected to MQTT Broker');
  client.subscribe(config.MQTT_TOPIC, (err) => {
    if (err) console.error('❌ MQTT Subscription Error:', err);
  });
});

// Listen for incoming MQTT messages and save them to PostgreSQL
// eslint-disable-next-line @typescript-eslint/no-misused-promises
client.on('message', async (_topic: string, message: Buffer) => {
  try {
    const data: MQTTData = JSON.parse(message.toString()) as MQTTData;

    const device = await prisma.device.findUnique({
      where: { uuid: data.deviceId },
    });

    await prisma.timeseries.createMany({
      data: data.data.map((element) => ({
        value: element.value,
        identifier: element.identifier,
        name: element.name,
        deviceId: data.deviceId,
        locationId: device?.locationUuid ?? '',
      })),
    });

    console.log(`✅ Data saved: ${JSON.stringify(data)}`);

    return;
  } catch (err) {
    console.error('❌ Database Error:', err);
  }
});
