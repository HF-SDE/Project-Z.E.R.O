// components/TopBar.tsx
import React from "react";
import {
  DropdownMenu,
  DropdownMenuTrigger,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuSeparator,
  DropdownMenuLabel,
} from "@components/ui/dropdown-menu"; // Dropdown components
import { ChevronDown } from "lucide-react"; // Icon for the dropdown arrow
import { Button } from "@components/ui/button"; // Your custom button

interface TopBarProps {
  pageTitle: string; // The page title that you want to display
}

const TopBar: React.FC<TopBarProps> = ({ pageTitle }) => {
  return (
    <header className="flex justify-between items-center p-4 bg-gray-800 text-white">
      {/* Page Title */}
      <h1 className="text-xl font-semibold">{pageTitle}</h1>{" "}
      {/* Improved styling for title */}
      {/* Settings Dropdown */}
      <DropdownMenu>
        <DropdownMenuTrigger asChild>
          <Button
            variant="outline"
            size="sm"
            className="flex items-center space-x-2"
          >
            <span>Settings</span>
            <ChevronDown className="h-4 w-4" />
          </Button>
        </DropdownMenuTrigger>

        <DropdownMenuContent>
          <DropdownMenuLabel>Settings</DropdownMenuLabel>
          <DropdownMenuItem onClick={() => alert("Profile clicked")}>
            Profile
          </DropdownMenuItem>
          <DropdownMenuItem onClick={() => alert("Account clicked")}>
            Account
          </DropdownMenuItem>
          <DropdownMenuSeparator />
          <DropdownMenuItem onClick={() => alert("Logout clicked")}>
            Logout
          </DropdownMenuItem>
        </DropdownMenuContent>
      </DropdownMenu>
    </header>
  );
};

export default TopBar;
