import { dirname } from "path";
import { fileURLToPath } from "url";
import { FlatCompat } from "@eslint/eslintrc";
import tailwind from "eslint-plugin-tailwindcss";
import jsdoc from 'eslint-plugin-jsdoc';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const compat = new FlatCompat({
  baseDirectory: __dirname,
});

const eslintConfig = [
  ...compat.extends("next/core-web-vitals", "next/typescript"),
  ...tailwind.configs["flat/recommended"],
  ...jsdoc.configs['flat/recommended'],
  {
    ignore: ["node_modules", "dist", "**/*.json", "**/*.sh", "**/*.production", "**/*.css", "Dockerfile", "tailwind.config.ts"],
    plugins: {
      jsdoc,
    },
    rules: {
      "react"
    }
  }
];

export default eslintConfig;
