"use client";

import * as React from "react";
import {
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
} from "@components/ui/sidebar";
import { Bot } from "lucide-react";

export function TeamSwitcher() {
  // Static team info
  const team = {
    name: "IOT Corp",
    logo: <Bot className="size-4" />, // Render Bot icon component here
    plan: "Enterprise", // Example plan
  };

  return (
    <SidebarMenu>
      <SidebarMenuItem>
        {/* This is now a simple button showing the team logo and name */}
        <SidebarMenuButton
          size="lg"
          className="flex items-center gap-2 text-sm font-semibold cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-lg p-2"
          onClick={() => {
            // Optional: Add navigation or action for team click
            console.log("Team clicked");
          }}
        >
          {/* Team logo (rendering SquareTerminal component directly) */}
          <div className="flex aspect-square size-8 items-center justify-center rounded-lg bg-sidebar-primary text-sidebar-primary-foreground">
            {team.logo}
          </div>
          {/* Team name and plan */}
          <div className="grid flex-1 text-left">
            <span className="truncate font-semibold">{team.name}</span>
            <span className="truncate text-xs">{team.plan}</span>
          </div>
        </SidebarMenuButton>
      </SidebarMenuItem>
    </SidebarMenu>
  );
}
