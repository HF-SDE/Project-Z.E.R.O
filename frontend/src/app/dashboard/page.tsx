export const metadata = {
  title: "Dashboard",
  description: "Admin dashboard",
};

export default async function Dashboard() {
  return (
    <div className="flex h-screen flex-col items-center mt-24">
      <h1 className="mb-4 text-center text-4xl font-bold">
        Welcome to the Admin Dashboard
      </h1>
      <p className="text-xl text-gray-600">
        Manage your data and settings with ease
      </p>
    </div>
  );
}
