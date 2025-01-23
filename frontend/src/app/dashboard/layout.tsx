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
  return (
    <SidebarProvider>
      <AppSidebar />
      <main className="flex flex-col">
        {/* Topbar with dynamic background */}
        <div className="flex items-center gap-4 p-4 w-full bg-white dark:bg-gray-800">
          {/* Sidebar Trigger */}
          <SidebarTrigger />

          {/* Breadcrumb */}
          <Breadcrumb className="flex-1">
            <BreadcrumbList className="text-slate-950 dark:text-slate-50">
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

          {/* Middle Dropdown to show the Data Table (Notification Bell Icon) */}
          <DropdownMenu>
            <DropdownMenuTrigger asChild>
              <button className="flex items-center justify-center p-2 rounded-full bg-gray-200 dark:bg-gray-600 hover:bg-gray-300 dark:hover:bg-gray-500 transition-all">
                <Bell className="h-6 w-6 text-gray-800 dark:text-white" />
              </button>
            </DropdownMenuTrigger>

            <DropdownMenuContent align="end">
              {/* Data Table inside the dropdown */}
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

          {/* Profile Avatar (rightmost icon) */}
          <DropdownMenu>
            <DropdownMenuTrigger asChild>
              <button className="flex items-center justify-center h-10 w-10 rounded-full bg-gray-200 dark:bg-gray-600 hover:bg-gray-300 dark:hover:bg-gray-500 transition-all">
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
        {/* Main content */}
        <div className="flex-1 p-4">{children}</div>{" "}
        {/* Page content will be displayed here */}
      </main>
    </SidebarProvider>
  );
}
