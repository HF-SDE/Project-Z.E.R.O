"use client"; // Add this directive at the top of the file

export default function Home() {
  return (
    <div className="flex flex-col items-center justify-center min-h-screen px-8 sm:px-20 md:px-40 font-[family-name:var(--font-geist-sans)]">
      {/* Main content section */}
      <main className="flex flex-col items-center justify-center flex-grow text-center w-full max-w-screen-lg">
        <h1 className="text-3xl sm:text-4xl font-semibold">
          Welcome to Your Dashboard
        </h1>
      </main>
    </div>
  );
}
