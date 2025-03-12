-- CreateTable
CREATE TABLE "TimeseriesAbnormal" (
    "id" SERIAL NOT NULL,
    "deviceId" UUID NOT NULL,
    "name" TEXT NOT NULL,
    "identifier" "Identifier" NOT NULL,
    "min" DOUBLE PRECISION NOT NULL,
    "max" DOUBLE PRECISION NOT NULL,
    "createdAt" TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,
    "updatedAt" TIMESTAMPTZ NOT NULL,

    CONSTRAINT "TimeseriesAbnormal_pkey" PRIMARY KEY ("id")
);

-- AddForeignKey
ALTER TABLE "TimeseriesAbnormal" ADD CONSTRAINT "TimeseriesAbnormal_deviceId_fkey" FOREIGN KEY ("deviceId") REFERENCES "Device"("uuid") ON DELETE RESTRICT ON UPDATE CASCADE;
