import SystemClientPage from "./client";
import { Data } from "./helper.function";

interface PageProps {
  searchParams: Promise<{
    location?: string;
    zone?: string;
  }>;
}

export default async function NewDashboard({ searchParams }: PageProps) {
  const { location, zone } = await searchParams;

  const data: Data = { locations: [], clients: [] };

  return (
    <SystemClientPage preLocation={location} preZone={zone} initData={data} />
  );
}
