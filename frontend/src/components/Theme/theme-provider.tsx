'use client';
import dynamic from "next/dynamic";
import { ComponentProps } from "react";
const NextThemesProvider = dynamic(
	() => import('next-themes').then((e) => e.ThemeProvider),
	{
		ssr: false,
	}
)

export default function ThemeProvider({
  children,
  ...props
}: ComponentProps<typeof NextThemesProvider>) {
  return <NextThemesProvider {...props}>{children}</NextThemesProvider>;
}
