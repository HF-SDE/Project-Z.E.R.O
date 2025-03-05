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
import { ChartContainer } from "@/components/ui/chart";

import axios from "axios";
import {
  Bar,
  BarChart,
  CartesianGrid,
  LabelList,
  Tooltip,
  XAxis,
  YAxis,
} from "recharts";

interface ItemData {
  name: string;
  count: number;
}

interface ItemsBarChartProps {
  order?: "most" | "least";
}

export function SBarChart({ order = "most" }: ItemsBarChartProps) {
  const [itemsData, setItemsData] = useState<ItemData[]>([]);
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    async function fetchData() {
      try {
        const response = await axios.get("/api/stats");
        const orderedStats = response.data.data.menuItems
          .orderedStats as ItemData[];
        const sortedItems = [...orderedStats].sort((a, b) =>
          order === "most" ? b.count - a.count : a.count - b.count,
        );
        const topFiveItems = sortedItems.slice(0, 5);
        setItemsData(topFiveItems);
      } catch (error) {
        console.error("Error fetching ordered items data:", error);
      } finally {
        setLoading(false);
      }
    }
    fetchData();
  }, [order]);

  if (loading) {
    return <div>Loading...</div>;
  }

  return (
    <Card>
      <CardHeader>
        <CardTitle>Ordered Items Bar Chart</CardTitle>
        <CardDescription>
          Top 5 {order === "most" ? "Most" : "Least"} Ordered Items
        </CardDescription>
      </CardHeader>
      <CardContent>
        <ChartContainer config={{}} className="h-[225px] w-full">
          <BarChart
            data={itemsData}
            margin={{ top: 20 }}
            className="h-full w-full"
          >
            <CartesianGrid vertical={false} stroke="var(--color-grid)" />
            <XAxis dataKey="name" stroke="var(--color-axis)" />
            <YAxis stroke="var(--color-axis)" />
            <Tooltip
              wrapperStyle={{
                backgroundColor: "rgba(0, 0, 0, 0.8)",
                color: "white",
              }}
            />
            <Bar dataKey="count" fill="hsl(var(--chart-2))" radius={8}>
              <LabelList
                position="top"
                offset={12}
                fontSize={12}
                fill="currentColor"
                className="text-foreground"
              />
            </Bar>
          </BarChart>
        </ChartContainer>
      </CardContent>
    </Card>
  );
}
