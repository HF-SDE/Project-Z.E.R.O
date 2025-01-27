"use client";

import * as React from "react";
import {
  AudioWaveform,
  Bot,
  Command,
  GalleryVerticalEnd,
  Settings2,
  SquareTerminal,
  Bell, // Alerts icon
  BarChart, // Icon for Statistics
} from "lucide-react";

import { NavMain } from "@components/nav-main";
import { NavUser } from "@components/nav-user";
import { TeamSwitcher } from "@components/team-switcher";
import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarRail,
} from "@components/ui/sidebar";

// This is sample data.
const data = {
  user: {
    name: "Admin",
    email: "Admin@IOT.com",
    avatar: "/avatars/shadcn.jpg",
  },
  navMain: [
    {
      title: "Home",
      url: "#",
      icon: SquareTerminal,
      isActive: true,
    },
    {
      title: "Devices",
      url: "/dashboard/device",
      icon: Bot,
    },
    {
      title: "Statistics",
      url: "#",
      icon: BarChart,
    },
    {
      title: "Locations",
      url: "#",
      icon: Settings2,
    },
    {
      title: "Alerts", // Alerts as a simple link
      url: "#",
      icon: Bell,
    },
  ],
};

export function AppSidebar({ ...props }: React.ComponentProps<typeof Sidebar>) {
  return (
    <Sidebar collapsible="icon" {...props}>
      <SidebarHeader>
        <TeamSwitcher />
      </SidebarHeader>
      <SidebarContent>
        <NavMain items={data.navMain} />
      </SidebarContent>
      <SidebarFooter>
        <NavUser user={data.user} />
      </SidebarFooter>
      <SidebarRail />
    </Sidebar>
  );
}
