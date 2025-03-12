CREATE TABLE IF NOT EXISTS iot_data.sensor_data_pg
ENGINE = PostgreSQL(
  'db:5432',
  '${POSTGRES_DB}',
  'Timeseries',
  '${POSTGRES_USER}',
  '${POSTGRES_PASSWORD}'
);