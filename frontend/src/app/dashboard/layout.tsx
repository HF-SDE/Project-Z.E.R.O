"use client";

import * as React from "react";
import { SidebarProvider, SidebarTrigger } from "@components/ui/sidebar";
import { AppSidebar } from "@components/app-sidebar";
import {
  Breadcrumb,
  BreadcrumbList,
  BreadcrumbItem,
  BreadcrumbLink,
  BreadcrumbPage,
  BreadcrumbSeparator,
} from "@components/ui/breadcrumb"; // Breadcrumb components
import { Avatar, AvatarImage, AvatarFallback } from "@components/ui/avatar"; // Avatar component
import {
  DropdownMenu,
  DropdownMenuTrigger,
  DropdownMenuContent,
  DropdownMenuItem,
} from "@components/ui/dropdown-menu"; // Dropdown Menu components
import { ModeToggle } from "@components/ui/theme-toggle"; // Light/Dark Mode Toggle
import {
  Table,
  TableHeader,
  TableBody,
  TableFooter,
  TableRow,
  TableHead,
  TableCell,
  TableCaption,
} from "@components/ui/table"; // Data table components
import { Bell } from "@components/Icons/Bell"; // Notification Bell Icon

export default function Layout({ children }: { children: React.ReactNode }) {
  const [sidebarOpen, setSidebarOpen] = React.useState(true);

  const toggleSidebar = () => {
    setSidebarOpen(!sidebarOpen);
  };

  return (
    <SidebarProvider>
      {/* Sidebar: Fixed and full height */}
      <AppSidebar
        className={`fixed top-0 left-0 h-full ${
          sidebarOpen ? "w-64" : "w-16"
        } bg-gray-800 dark:bg-gray-900 z-20`}
        style={{
          borderColor: `hsl(var(--sidebar-border))`,
        }}
      />
      <main className="flex flex-col dark:bg-slate-900 min-h-screen">
        {/* Topbar: Positioned starting after the sidebar */}
        <div
          className="fixed top-0 z-10 flex items-center gap-4 p-4 transition-all duration-300"
          style={{
            backgroundColor: `hsl(var(--background))`,
            color: `hsl(var(--foreground))`,
            left: sidebarOpen ? "16rem" : "3rem", // Adjust the left position based on sidebar state
            width: `calc(100% - ${sidebarOpen ? "16rem" : "3rem"})`, // Adjust the width of the topbar dynamically
          }}
        >
          {/* Sidebar Trigger */}
          <SidebarTrigger onClick={toggleSidebar} />

          {/* Breadcrumb */}
          <Breadcrumb className="flex-1">
            <BreadcrumbList>
              <BreadcrumbItem>
                <BreadcrumbLink href="#">Home</BreadcrumbLink>
              </BreadcrumbItem>
              <BreadcrumbSeparator />
              <BreadcrumbItem>
                <BreadcrumbLink href="#">Dashboard</BreadcrumbLink>
              </BreadcrumbItem>
              <BreadcrumbSeparator />
              <BreadcrumbItem>
                <BreadcrumbPage>Page Title</BreadcrumbPage>
              </BreadcrumbItem>
            </BreadcrumbList>
          </Breadcrumb>

          {/* Dark Mode Toggle */}
          <ModeToggle />

          {/* Notification Dropdown */}
          <DropdownMenu>
            <DropdownMenuTrigger asChild>
              <button className="flex items-center justify-center p-2 rounded-full bg-gray-200 dark:bg-gray-700 hover:bg-gray-300 dark:hover:bg-gray-600 transition-all">
                <Bell className="h-6 w-6 text-gray-800 dark:text-white" />
              </button>
            </DropdownMenuTrigger>
            <DropdownMenuContent align="end">
              <Table>
                <TableHeader>
                  <TableRow>
                    <TableHead>Name</TableHead>
                    <TableHead>Age</TableHead>
                    <TableHead>Email</TableHead>
                    <TableHead>Role</TableHead>
                  </TableRow>
                </TableHeader>
                <TableBody>
                  <TableRow>
                    <TableCell>John Doe</TableCell>
                    <TableCell>30</TableCell>
                    <TableCell>johndoe@example.com</TableCell>
                    <TableCell>Admin</TableCell>
                  </TableRow>
                  <TableRow>
                    <TableCell>Jane Smith</TableCell>
                    <TableCell>25</TableCell>
                    <TableCell>janesmith@example.com</TableCell>
                    <TableCell>User</TableCell>
                  </TableRow>
                  <TableRow>
                    <TableCell>Sam Green</TableCell>
                    <TableCell>28</TableCell>
                    <TableCell>samgreen@example.com</TableCell>
                    <TableCell>Moderator</TableCell>
                  </TableRow>
                </TableBody>
              </Table>
            </DropdownMenuContent>
          </DropdownMenu>

          {/* Profile Avatar */}
          <DropdownMenu>
            <DropdownMenuTrigger asChild>
              <button className="flex items-center justify-center h-10 w-10 rounded-full bg-gray-200 dark:bg-gray-700 hover:bg-blue-100 dark:hover:bg-blue-800 transition-colors duration-200 ease-in-out">
                <Avatar>
                  <AvatarImage
                    src="/path/to/your/profile-pic.jpg"
                    alt="Profile"
                  />
                  <AvatarFallback>JD</AvatarFallback>
                </Avatar>
              </button>
            </DropdownMenuTrigger>
            <DropdownMenuContent align="end">
              <DropdownMenuItem>Edit Profile</DropdownMenuItem>
              <DropdownMenuItem>Settings</DropdownMenuItem>
              <DropdownMenuItem>Logout</DropdownMenuItem>
            </DropdownMenuContent>
          </DropdownMenu>
        </div>

        {/* Content: Padding to avoid overlap */}
        <div className="ml-64 pt-[4rem] p-4 flex-grow">{children}</div>
      </main>
    </SidebarProvider>
  );
}
