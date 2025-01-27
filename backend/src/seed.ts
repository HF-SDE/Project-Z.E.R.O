import { hash } from 'argon2';

import { faker } from '@faker-js/faker';
import { Prisma, PrismaClient } from '@prisma/client';

// Use crypto to generate random hex strings

const prisma = new PrismaClient();
// eslint-disable-next-line @typescript-eslint/no-unsafe-assignment, @typescript-eslint/no-unsafe-call

/**
 * Used for generating test data to mongo database
 * PSQL database is used in the app and management side
 */
async function generatePSQL() {
  // Create Permission Groups
  await prisma.permissionGroup.createMany({
    data: [
      { name: 'Administrator' },
      { name: 'Device' },
      { name: 'Location' },
      { name: 'Alert' },
      { name: 'Data' },
    ],
  });

  // Create Permission
  await prisma.permission.createMany({
    data: [
      {
        code: 'administrator:users:view',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'See users information',
      },
      {
        code: 'administrator:users:update',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'Update users information',
      },
      {
        code: 'administrator:users:create',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'Create new users',
      },
      {
        code: 'administrator:users:management',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'Give user permissions',
      },
      {
        code: 'administrator:permission:create',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'Create new permission',
      },
      {
        code: 'administrator:permission:view',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'View permissions and their groups',
      },
      {
        code: 'administrator:permissiongroup:create',
        permissionGroupId: await findPermissionGroup('Administrator'),
        description: 'Create new permission group',
      },
      {
        code: 'device:view',
        permissionGroupId: await findPermissionGroup('Device'),
        description: 'View devices',
      },
      {
        code: 'device:create',
        permissionGroupId: await findPermissionGroup('Device'),
        description: 'Create new devices',
      },
      {
        code: 'device:update',
        permissionGroupId: await findPermissionGroup('Device'),
        description: 'Update device information',
      },
      {
        code: 'device:delete',
        permissionGroupId: await findPermissionGroup('Device'),
        description: 'Delete device',
      },
      {
        code: 'location:view',
        permissionGroupId: await findPermissionGroup('Location'),
        description: 'View location information',
      },
      {
        code: 'location:create',
        permissionGroupId: await findPermissionGroup('Location'),
        description: 'Create new location',
      },
      {
        code: 'location:update',
        permissionGroupId: await findPermissionGroup('Location'),
        description: 'Update location information',
      },
      {
        code: 'location:delete',
        permissionGroupId: await findPermissionGroup('Location'),
        description: 'Delete location',
      },
      {
        code: 'alert:view',
        permissionGroupId: await findPermissionGroup('Alert'),
        description: 'View alert information',
      },
      {
        code: 'alert:create',
        permissionGroupId: await findPermissionGroup('Alert'),
        description: 'Create new alert',
      },
      {
        code: 'alert:update',
        permissionGroupId: await findPermissionGroup('Alert'),
        description: 'Update alert information',
      },
      {
        code: 'data:view',
        permissionGroupId: await findPermissionGroup('Data'),
        description: 'View data information',
      },
    ],
  });

  // Create super admin user
  const superAdminUser = await prisma.user.create({
    data: {
      initials: 'AD',
      username: 'admin',
      password: await hash('admin'),
      name: 'Super Admin',
      email: 'admin@example.com',
    },
  });

  const technicianUser = await prisma.user.create({
    data: {
      initials: 'TC',
      username: 'technician',
      password: await hash('technician'),
      name: 'Technician',
      email: 'technician@example.com',
    },
  });

  // Assign permissions to super admin user
  const adminPermission = await prisma.permission.findMany();

  if (adminPermission) {
    for (const permission of adminPermission) {
      await prisma.userPermissions.create({
        data: {
          userId: superAdminUser.id,
          assignedBy: superAdminUser.id,
          permissionId: permission.id,
        },
      });
    }
  }

  const technicianPermissionTransaction = [];

  const technicianPermissions = await prisma.permission.findMany({
    where: { PermissionGroup: { OR: [{ name: 'Device' }, { name: 'Data' }] } },
  });

  for (const permission of technicianPermissions) {
    const prismaTransaction = prisma.userPermissions.create({
      data: {
        userId: technicianUser.id,
        assignedBy: superAdminUser.id,
        permissionId: permission.id,
      },
    });

    technicianPermissionTransaction.push(prismaTransaction);
  }

  await prisma.$transaction(technicianPermissionTransaction);

  // Create location
  await prisma.location.createMany({
    data: [
      {
        name: 'Stue',
      },
      {
        name: 'Gang',
      },
    ],
  });

  // Create device
  await prisma.device.createMany({
    data: [
      {
        name: 'Device 1',
        frequency: 120,
        locationUuid: await findLocation('Stue'),
      },
      {
        name: 'Device 2',
        frequency: 120,
        locationUuid: await findLocation('Gang'),
      },
    ],
  });

  // Create alert
  await prisma.alert.createMany({
    data: [
      {
        deviceId: await findDevice('Device 1'),
        type: 'ERROR',
        description: 'Temperature is too high',
        threshold: 30,
        identifier: 'CELSIUS',
        name: 'TEMPERATURE_TO_HIGH',
      },
      {
        deviceId: await findDevice('Device 2'),
        type: 'ERROR',
        description: 'Temperature is too low',
        threshold: 20,
        identifier: 'CELSIUS',
        name: 'TEMPERATURE_TO_LOW',
      },
    ],
  });

  // Create Data
  const timeseriesData: Prisma.TimeseriesCreateManyInput[] = [];

  for (let index = 0; (index = 20); index++) {
    if (Math.floor(Math.random() * 1) === 0) {
      timeseriesData.push({
        deviceId: await findDevice('Device 1'),
        value: faker.number.float({ max: 100 }),
        identifier: 'CELSIUS',
        locationId: await findLocation('Stue'),
      });
    } else {
      timeseriesData.push({
        deviceId: await findDevice('Device 2'),
        value: faker.number.float({ max: 100 }),
        identifier: 'CELSIUS',
        locationId: await findLocation('Gang'),
      });
    }
  }

  await prisma.timeseries.createMany({
    data: timeseriesData,
  });

  /**
   * Find device id by name
   * @async
   * @param {string} name - Device name
   * @returns {Promise<string>} - Device uuid
   */
  async function findDevice(name: string): Promise<string> {
    try {
      const result = await prisma.device.findFirst({
        where: { name },
      });

      if (result) return result.uuid;
      throw new Error('Device not found');
    } catch (error: unknown) {
      return (error as Error).message;
    }
  }

  /**
   * Find location id by name
   * @async
   * @param {string} name - Location name
   * @returns {Promise<string>} - Location uuid
   */
  async function findLocation(name: string): Promise<string> {
    try {
      const result = await prisma.location.findFirst({
        where: { name },
      });

      if (result) return result.uuid;
      throw new Error('Location not found');
    } catch (error: unknown) {
      return (error as Error).message;
    }
  }

  /**
   * Find permission group id by name
   * @async
   * @param {string} name Permission group name
   * @returns {Promise<string>} Permission group id
   */
  async function findPermissionGroup(name: string): Promise<string> {
    try {
      const result = await prisma.permissionGroup.findFirst({
        where: { name },
        select: { id: true },
      });

      if (result) return result.id;
      throw new Error('Permission group not found');
    } catch (error: unknown) {
      return (error as Error).message;
    }
  }
}

/**
 * Generate data to PostgreSQL
 */
generatePSQL()
  .then(async () => {
    await prisma.$disconnect();
  })
  .catch(async (e) => {
    console.error(e);
    await prisma.$disconnect();
    process.exit(1);
  });
