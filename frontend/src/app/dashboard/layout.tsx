import { cookies } from "next/headers";

import { AppSidebar } from "@/components/app-sidebar";
import DynamicBreadcrumbs from "@/components/dynamic-breadcrumbs";
// import RealtimeLogout from "@/components/realtimeLogout";
import "@/components/ui/breadcrumb";
import { Separator } from "@/components/ui/separator";
import {
  SidebarInset,
  SidebarProvider,
  SidebarTrigger,
} from "@/components/ui/sidebar";

import { getUser } from "@/helpers/getUser";

import UserProvider from "@/providers/user";

export const metadata = {
  title: "Dashboard",
  description: "Admin dashboard",
};

interface LayoutProps {
  children: React.ReactNode;
}

export default async function Layout({ children }: LayoutProps) {
  const cookieStore = await cookies();
  const defaultOpen = cookieStore.get("sidebar:state")?.value === "true";

  const user = await getUser();

  if (!user) return;

  return (
    <UserProvider initUser={user}>
      {/* <RealtimeLogout /> */}
      <SidebarProvider defaultOpen={defaultOpen}>
        <AppSidebar />
        <SidebarInset>
          <header className="flex h-16 shrink-0 items-center gap-2 transition-[width,height] ease-linear group-has-[[data-collapsible=icon]]/sidebar-wrapper:h-12">
            <div className="flex items-center gap-2 px-4">
              <SidebarTrigger className="-ml-1" />
              <Separator orientation="vertical" className="mr-2 h-4" />
              <DynamicBreadcrumbs />
            </div>
          </header>
          {children}
        </SidebarInset>
      </SidebarProvider>
    </UserProvider>
  );
}
