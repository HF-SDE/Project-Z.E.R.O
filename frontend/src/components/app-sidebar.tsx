"use client";

import * as React from "react";

import { NavProjects } from "@/components/nav-projects";
import { NavUser } from "@/components/nav-user";
// import { TeamSwitcher } from "@/components/team-switcher";
import {
  Sidebar,
  SidebarContent,
  SidebarFooter,
  SidebarHeader,
  SidebarRail,
} from "@/components/ui/sidebar";

import {
  ChartArea,
  ChefHat,
  GalleryVerticalEnd,
  House,
  Users,
} from "lucide-react";

const data = {
  teams: [{ name: "Acme Inc", logo: GalleryVerticalEnd, plan: "Enterprise" }],
  pages: [
    {
      name: "Home",
      url: "/dashboard",
      icon: House,
    },
    {
      name: "Users",
      url: "/dashboard/users",
      icon: Users,
      neededPermissions: ["administrator:users:view"],
    },
    {
      name: "Menu",
      url: "/dashboard/menu",
      icon: ChefHat,
    },
    {
      name: "Stats",
      url: "/dashboard/stats",
      icon: ChartArea,
      neededPermissions: ["administrator:stats:view"],
    },
  ],
};

export function AppSidebar({ ...props }: React.ComponentProps<typeof Sidebar>) {
  return (
    <Sidebar collapsible="icon" {...props}>
      <SidebarHeader>{/* <TeamSwitcher teams={data.teams} /> */}</SidebarHeader>
      <SidebarContent>
        <NavProjects pages={data.pages} />
      </SidebarContent>
      <SidebarFooter>
        <NavUser />
      </SidebarFooter>
      <SidebarRail />
    </Sidebar>
  );
}
