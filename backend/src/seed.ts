import { hash } from 'argon2';

import { PrismaClient } from '@prisma/client';

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
    data: [{ name: 'Administrator' }, { name: 'Location' }],
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

  /**
   * Find permission group id by name
   * @async
   * @param {string} name Permission group name
   * @returns {Promise<string>} Permission group id
   */
  async function findPermissionGroup(name: string): Promise<string> {
    try {
      const result = await prisma.permissionGroup.findFirst({
        where: {
          name,
        },
        select: {
          id: true,
        },
      });
      if (result) {
        return result.id;
      }
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
