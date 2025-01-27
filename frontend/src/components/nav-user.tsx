"use client";

import {
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
} from "@components/ui/sidebar";
import { SquareTerminal } from "lucide-react";

export function NavUser({
  user,
}: {
  user: {
    name: string;
    email: string;
    avatar: string;
  };
}) {
  return (
    <SidebarMenu>
      <SidebarMenuItem>
        {/* NavUser button */}
        <SidebarMenuButton
          size="lg"
          className="flex items-center gap-2 text-sm font-semibold cursor-pointer hover:bg-gray-200 dark:hover:bg-gray-700 rounded-lg p-2"
          onClick={() => {
            // Optional: Add navigation or action for user click
            console.log("User clicked");
          }}
        >
          {/* User avatar icon */}
          <div className="flex aspect-square h-8 w-8 items-center justify-center rounded-lg bg-sidebar-primary text-sidebar-primary-foreground">
            <SquareTerminal className="text-white" />
          </div>

          {/* User name and email */}
          <div className="grid flex-1 text-left">
            <span className="truncate font-semibold">{user.name}</span>
            <span className="truncate text-xs">{user.email}</span>
          </div>
        </SidebarMenuButton>
      </SidebarMenuItem>
    </SidebarMenu>
  );
}
