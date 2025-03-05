"use client";

import * as React from "react";
import { useEffect, useState } from "react";

import {
  Card,
  CardContent,
  CardDescription,
  CardFooter,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import { ChartConfig, ChartContainer } from "@/components/ui/chart";

import axios from "axios";
import {
  Label,
  PolarGrid,
  PolarRadiusAxis,
  RadialBar,
  RadialBarChart,
} from "recharts";

export function SRadialChart() {
  // State to store reservations data.
  const [reservations, setReservations] = useState<{
    total: number;
    today: number;
    upcoming: number;
    tableUtilizationPercentage: number;
  } | null>(null);

  useEffect(() => {
    async function fetchData() {
      try {
        const { data } = await axios.get("/api/stats");
        // Assuming the API returns:
        // { data: { reservations: { total, today, upcoming, tableUtilizationPercentage } } }
        setReservations(data.data.reservations);
      } catch (error) {
        console.error("Error fetching reservations data:", error);
      }
    }
    fetchData();
  }, []);

  if (!reservations) {
    return <div>Loading...</div>;
  }

  // Prepare chart data using the table utilization percentage.
  const chartData = [
    {
      utilization: reservations.tableUtilizationPercentage,
      fill: "hsl(var(--chart-1))",
    },
  ];

  // Optional chart configuration.
  const chartConfig = {
    utilization: {
      label: "Table Utilization",
      color: "hsl(var(--chart-1))",
    },
  } satisfies ChartConfig;

  // Compute dynamic endAngle.
  // Here we use a startAngle of 90 and compute an endAngle so that the arc represents the percentage.
  const startAngle = 90;
  const endAngle = 90 - (reservations.tableUtilizationPercentage / 100) * 360;

  return (
    <Card className="flex flex-col">
      <CardHeader className="items-center pb-0">
        <CardTitle>Reservations</CardTitle>
        <CardDescription>Current Table Utilization</CardDescription>
      </CardHeader>
      <CardContent className="flex-1 pb-0">
        <ChartContainer config={chartConfig} className="ax-h-full mx-auto">
          <RadialBarChart
            data={chartData}
            startAngle={startAngle}
            endAngle={endAngle}
            innerRadius={80}
            outerRadius={140}
          >
            <PolarGrid
              gridType="circle"
              radialLines={false}
              stroke="none"
              className="first:fill-muted last:fill-background"
              polarRadius={[86, 74]}
            />
            <RadialBar
              dataKey="utilization"
              fill="hsl(var(--chart-1))"
              background
            />
            <PolarRadiusAxis tick={false} tickLine={false} axisLine={false}>
              <Label
                content={({ viewBox }) => {
                  if (viewBox && "cx" in viewBox && "cy" in viewBox) {
                    return (
                      <text
                        x={viewBox.cx}
                        y={viewBox.cy}
                        textAnchor="middle"
                        dominantBaseline="middle"
                      >
                        <tspan className="fill-foreground text-4xl font-bold">
                          {reservations.tableUtilizationPercentage.toLocaleString()}
                          %
                        </tspan>
                        <tspan
                          x={viewBox.cx}
                          y={(viewBox.cy || 0) + 24}
                          className="fill-muted-foreground text-sm"
                        >
                          Table Utilization
                        </tspan>
                      </text>
                    );
                  }
                  return null;
                }}
              />
            </PolarRadiusAxis>
          </RadialBarChart>
        </ChartContainer>
      </CardContent>
      <CardFooter className="flex flex-col gap-2 text-sm">
        <div className="font-medium">
          Total Reservations: {reservations.total.toLocaleString()}
        </div>
        <div className="text-muted-foreground">
          Today: {reservations.today} &bull; Upcoming: {reservations.upcoming}
        </div>
      </CardFooter>
    </Card>
  );
}
