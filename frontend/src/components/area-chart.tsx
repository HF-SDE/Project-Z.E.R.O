"use client";

import * as React from "react";
import { useEffect, useState } from "react";

import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "@/components/ui/card";
import {
  ChartConfig,
  ChartContainer,
  ChartLegend,
  ChartLegendContent,
  ChartTooltip,
  ChartTooltipContent,
} from "@/components/ui/chart";
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";

import axios from "axios";
import { Area, AreaChart, CartesianGrid, XAxis } from "recharts";

const chartConfig = {
  sales: {
    label: "Monthly Sales",
    color: "hsl(var(--chart-1))",
  },
} satisfies ChartConfig;

export function SAreaChart() {
  const [chartData, setChartData] = useState<
    Array<{ date: string; sales: number }>
  >([]);
  const [timeRange, setTimeRange] = useState("all");

  useEffect(() => {
    async function fetchStats() {
      try {
        const { data: apiData } = await axios.get("/api/stats");
        const salesMonth = apiData.data.economy.salesMonth;

        // Mapping from month abbreviation to two-digit month numbers.
        const monthMapping: Record<string, string> = {
          Jan: "01",
          Feb: "02",
          Mar: "03",
          Apr: "04",
          May: "05",
          Jun: "06",
          Jul: "07",
          Aug: "08",
          Sep: "09",
          Oct: "10",
          Nov: "11",
          Dec: "12",
        };

        const data = salesMonth.map(
          (item: { year: string; month: string; sales: number }) => {
            const monthNum = monthMapping[item.month] || "01";
            const dateStr = `${item.year}-${monthNum}-01`;
            return {
              date: dateStr,
              sales: item.sales,
            };
          },
        );

        // Sort chronologically (oldest first).
        data.sort(
          (a, b) => new Date(a.date).getTime() - new Date(b.date).getTime(),
        );
        setChartData(data);
      } catch (error) {
        console.error("Error fetching monthly sales data:", error);
      }
    }
    fetchStats();
  }, []);

  const filteredData = React.useMemo(() => {
    if (timeRange === "all" || chartData.length === 0) return chartData;
    const latestDate = new Date(chartData[chartData.length - 1].date);
    let monthsToSubtract = 0;
    if (timeRange === "24m") monthsToSubtract = 24;
    else if (timeRange === "12m") monthsToSubtract = 12;
    else if (timeRange === "6m") monthsToSubtract = 6;
    const startDate = new Date(latestDate);
    startDate.setMonth(startDate.getMonth() - monthsToSubtract);
    return chartData.filter((item) => new Date(item.date) >= startDate);
  }, [chartData, timeRange]);

  return (
    <Card>
      <CardHeader className="flex items-center gap-2 space-y-0 border-b py-5 sm:flex-row">
        <div className="grid flex-1 gap-1 text-center sm:text-left">
          <CardTitle>Monthly Sales</CardTitle>
          <CardDescription>
            Showing monthly sales data from the API
          </CardDescription>
        </div>
        <Select value={timeRange} onValueChange={setTimeRange}>
          <SelectTrigger
            className="w-[160px] rounded-lg sm:ml-auto"
            aria-label="Select time range"
          >
            <SelectValue placeholder="All Time" />
          </SelectTrigger>
          <SelectContent className="rounded-xl">
            <SelectItem value="all" className="rounded-lg">
              All Time
            </SelectItem>
            <SelectItem value="24m" className="rounded-lg">
              Last 24 Months
            </SelectItem>
            <SelectItem value="12m" className="rounded-lg">
              Last 12 Months
            </SelectItem>
            <SelectItem value="6m" className="rounded-lg">
              Last 6 Months
            </SelectItem>
          </SelectContent>
        </Select>
      </CardHeader>
      <CardContent className="px-2 pt-4 sm:px-6 sm:pt-6">
        {/* Increased height for the area chart */}
        <ChartContainer config={chartConfig} className="h-[300px] w-full">
          <AreaChart data={filteredData}>
            <defs>
              <linearGradient id="fillSales" x1="0" y1="0" x2="0" y2="1">
                <stop
                  offset="5%"
                  stopColor="hsl(var(--chart-1))"
                  stopOpacity={0.8}
                />
                <stop
                  offset="95%"
                  stopColor="hsl(var(--chart-1))"
                  stopOpacity={0.1}
                />
              </linearGradient>
            </defs>
            <CartesianGrid vertical={false} stroke="var(--color-grid)" />
            <XAxis
              dataKey="date"
              tickLine={false}
              axisLine={false}
              tickMargin={8}
              minTickGap={32}
              tickFormatter={(value) => {
                const date = new Date(value);
                return date.toLocaleDateString("en-US", {
                  month: "short",
                  year: "numeric",
                });
              }}
              stroke="var(--color-axis)"
            />
            <ChartTooltip
              cursor={false}
              content={
                <ChartTooltipContent
                  labelFormatter={(value) => {
                    const date = new Date(value);
                    return date.toLocaleDateString("en-US", {
                      month: "short",
                      year: "numeric",
                    });
                  }}
                  indicator="dot"
                />
              }
            />
            <Area
              dataKey="sales"
              type="monotone"
              fill="url(#fillSales)"
              stroke="hsl(var(--chart-1))"
            />
            <ChartLegend content={<ChartLegendContent />} />
          </AreaChart>
        </ChartContainer>
      </CardContent>
    </Card>
  );
}
