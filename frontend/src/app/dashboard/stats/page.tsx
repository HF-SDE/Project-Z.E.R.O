"use client";

import React from "react";

import { SAreaChart } from "@/components/area-chart";
import { SBarChart } from "@/components/bar-chart";
import { SRadialChart } from "@/components/radial-chart";

export default function StatsPage() {
  return (
    <div className="p-4">
      <div className="flex h-full w-full flex-col flex-wrap gap-4">
        <div className="">
          <SAreaChart />
        </div>
        <div className="flex flex-wrap gap-4 md:gap-6">
          <div className="min-w-[300px] flex-1">
            <SRadialChart />
          </div>
          <div className="min-w-[300px] flex-1">
            <SBarChart order="most" />
          </div>
          <div className="min-w-[300px] flex-1">
            <SBarChart order="least" />
          </div>
        </div>
      </div>
    </div>
  );
}
