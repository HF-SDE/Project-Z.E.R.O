import { afterAll, beforeAll, describe, expect, it } from 'vitest';

import { APIResponse } from '@api-types/general.types';
import prisma from '@prisma-instance';
import { MenuItem, Prisma } from '@prisma/client';
import { DefaultArgs } from '@prisma/client/runtime/library';

import { axiosInstance as axios } from './axiosInstance';
import { login, logout } from './util';

type GetId = string | (() => Promise<{ id: string }>);

interface ExpectedMessages {
  getAll: string;
  create: string;
  update: string;
  delete: string;
}

export default class TestCases<T extends { id: string }> {
  url: string;
  expectedMessage: Partial<ExpectedMessages>;
  cleanupList: string[] = [];

  constructor(url: string, expectedMessage: Partial<ExpectedMessages> = {}) {
    this.url = url;
    this.expectedMessage = expectedMessage;
  }

  cleanUp() {
    if (!this) return;

    for (const id of this.cleanupList) {
      axios.delete(`${this.url}/${id}`);
    }
  }

  getAllTest() {
    return async () => {
      const response = await axios.get<{ data: APIResponse<T[]> }>(this.url);

      expect(response.status).toBe(200);
      expect(response.data).toEqual({
        data: expect.arrayContaining([]),
        status: 'Found',
        message: this.expectedMessage?.getAll,
      });
    };
  }

  getOneTest(getId: GetId) {
    return async () => {
      let id = getId;

      if (typeof getId === 'function') id = (await getId()).id;

      const response = await axios.get<{ data: APIResponse<T> }>(
        `${this.url}/${id}`,
      );

      expect(response.status).toBe(200);
      expect(response.data).toEqual({
        data: expect.arrayContaining([]),
        status: 'Found',
        message: this.expectedMessage?.getAll,
      });
    };
  }

  createTest(body: any) {
    return async () => {
      const response = await axios.post<{ data: APIResponse<T> }>(
        this.url,
        body,
      );

      if (response.status === 201) {
        const getAllResponse = await axios.get<APIResponse<T[]>>(this.url);
        const getAll = getAllResponse.data.data;

        if (!getAll) return;
        this.cleanupList.push(getAll[getAll.length - 1].id);
      }

      expect(response.status).toBe(201);
      expect(response.data).toStrictEqual({
        status: 'Created',
        message: this.expectedMessage?.create,
      });
    };
  }

  updateTest() {
    return async () => {};
  }

  deleteTest() {
    return async () => {};
  }
}
