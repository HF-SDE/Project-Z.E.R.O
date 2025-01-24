import { type LucideIcon } from "lucide-react";

import {
  SidebarGroup,
  SidebarMenu,
  SidebarMenuButton,
  SidebarMenuItem,
} from "@components/ui/sidebar";
import React from "react";

export function NavMain({
  items,
}: {
  items: {
    title: string;
    url: string;
    icon?: LucideIcon;
    isActive?: boolean;
    items?: {
      title: string;
      url: string;
    }[];
  }[];
}) {
  return (
    <SidebarGroup>
      <SidebarMenu>
        {items.map((item) => (
          <React.Fragment key={item.title}>
            {/* Main Menu Item */}
            <SidebarMenuItem>
              <SidebarMenuButton tooltip={item.title} asChild>
                <a
                  href={item.url}
                  className={`flex items-center gap-2 ${
                    item.isActive ? "text-white font-bold" : "text-gray-400"
                  }`}
                >
                  {item.icon && <item.icon />}
                  <span>{item.title}</span>
                </a>
              </SidebarMenuButton>
            </SidebarMenuItem>

            {/* Render Sub-Items as Separate Links */}
            {item.items?.map((subItem) => (
              <SidebarMenuItem>
                <SidebarMenuButton tooltip="Shaden" asChild>
                  <a
                    href="/shaden"
                    className="flex items-center gap-2 text-gray-300"
                  >
                    <span>Shaden</span>
                  </a>
                </SidebarMenuButton>
              </SidebarMenuItem>
            ))}
          </React.Fragment>
        ))}
      </SidebarMenu>
    </SidebarGroup>
  );
}
