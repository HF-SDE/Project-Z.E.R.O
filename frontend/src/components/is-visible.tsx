import { useEffect } from "react";

interface IsVisibleProps {
  value: boolean;
  toggleVisibility: (value: boolean) => void;
  children: React.ReactNode;
}

export function IsVisible({
  value,
  toggleVisibility,
  children,
}: IsVisibleProps) {
  useEffect(() => {
    toggleVisibility(value);
  }, [toggleVisibility, value]);

  return children;
}
