import bodyParser from 'body-parser';
import { log } from 'console';
import cors from 'cors';
import { rateLimit } from 'express-rate-limit';
import helmet from 'helmet';
import passport from 'passport';

import app from '@app';
import config from '@config';
import prisma from '@prisma-instance';
import { Identifier } from '@prisma/client';
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

  // Function to calculate IQR boundaries per group
  /**
   * Calculates the Interquartile Range (IQR) boundaries for a given set of values.
   * @param {number[]} values - The array of numbers to calculate the IQR boundaries for.
   * @returns {{ min: number, max: number }} The minimum and maximum boundaries based on the IQR.
   */
  function calculateBoundaries(values: number[]): { min: number; max: number } {
    if (values.length < 4) {
      return { min: Math.min(...values), max: Math.max(...values) }; // Fallback for small datasets
    }

    // Sort the values in ascending order
    const sortedValues = [...values].sort((a, b) => a - b);

    // Calculate the 1st Quartile (Q1) and 3rd Quartile (Q3)
    const Q1 = sortedValues[Math.floor((sortedValues.length + 1) / 4)];
    const Q3 = sortedValues[Math.floor((3 * (sortedValues.length + 1)) / 4)];

    // Calculate the Interquartile Range (IQR)
    const IQR = Q3 - Q1;

    // Calculate the boundaries (typically 1.5 * IQR beyond Q1 and Q3)
    const minBoundary = Q3 - 1.5 * IQR;
    const maxBoundary = Q1 + 1.5 * IQR;

    return { min: minBoundary, max: maxBoundary };
  }

  /**
   * Processes timeseries data to detect anomalies.
   */
  async function processTimeseriesData() {
    try {
      console.info('🔄 Running anomaly detection...');

      // Fetch all timeseries data
      const timeseriesData = await prisma.timeseries.findMany();

      // Group data by deviceId, identifier, and name
      const groupedData = timeseriesData.reduce(
        (acc, entry) => {
          const key = `${entry.deviceId}-${entry.identifier}-${entry.name}`;
          if (!acc[key]) {
            acc[key] = {
              deviceId: entry.deviceId,
              identifier: entry.identifier,
              name: entry.name,
              values: [entry.value],
            };
          } else {
            acc[key].values.push(entry.value);
          }
          return acc;
        },
        {} as Record<
          string,
          {
            deviceId: string;
            identifier: Identifier;
            name: string | null;
            values: number[];
          }
        >,
      );
      console.log(groupedData);

      // Process each grouped dataset
      for (const key in groupedData) {
        const { deviceId, identifier, name, values } = groupedData[key];

        // Calculate IQR min/max for this group
        const { min, max } = calculateBoundaries(values);

        // Check if a TimeseriesAbnormal entry exists
        const existingAbnormal = await prisma.timeseriesAbnormal.findFirst({
          where: { deviceId, identifier, name: name ?? '' },
        });

        log('min:', min);
        log('max:', max);

        if (existingAbnormal) {
          // Update existing abnormal record
          await prisma.timeseriesAbnormal.update({
            where: { id: existingAbnormal.id },
            data: { min, max, updatedAt: new Date() },
          });
        } else {
          // Create new abnormal record
          await prisma.timeseriesAbnormal.create({
            data: { deviceId, identifier, name: name ?? '', min, max },
          });
        }
      }

      console.info('✅ Anomaly detection completed.');
    } catch (err) {
      console.error('❌ Error in anomaly detection:', err);
    }
  }

  setInterval(() => {
    void processTimeseriesData();
  }, 10000);
});
