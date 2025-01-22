import { LoginForm } from "@components/login-form";

export default function Home() {
  return (
    <main className="flex min-h-svh w-full items-center justify-center p-6 bg-[#F4f4f6] bg-cover bg-fixed bg-light-login dark:bg-dark-login dark:bg-[#0c0b09] md:p-10">
      <section className="w-full max-w-sm">
        <LoginForm />
      </section>
    </main>
  );
}
