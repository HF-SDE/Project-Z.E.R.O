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
      "react-hooks/exhaustive-deps": "off",
      "@typescript-eslint/adjacent-overload-signatures": "error",
      "@typescript-eslint/array-type": [
        "error",
        {
          default: "array",
        },
      ],
      "@typescript-eslint/ban-types": [
        "error",
        {
          types: {
            Object: {
              message: "Avoid using the `Object` type. Did you mean `object`?",
            },
            Function: {
              message:
                "Avoid using the `Function` type. Prefer a specific function type, like `() => void`.",
            },
            Boolean: {
              message: "Avoid using the `Boolean` type. Did you mean `boolean`?",
            },
            Number: {
              message: "Avoid using the `Number` type. Did you mean `number`?",
            },
            String: {
              message: "Avoid using the `String` type. Did you mean `string`?",
            },
            Symbol: {
              message: "Avoid using the `Symbol` type. Did you mean `symbol`?",
            },
          },
        },
      ],
      "@typescript-eslint/consistent-type-assertions": "error",
      "@typescript-eslint/dot-notation": "error",
      "@typescript-eslint/explicit-function-return-type": "off",
      "@typescript-eslint/explicit-module-boundary-types": "off",
      "@typescript-eslint/naming-convention": [
        "error",
        {
          selector: "variable",
          format: ["camelCase", "UPPER_CASE", "PascalCase"],
          leadingUnderscore: "allow",
          trailingUnderscore: "forbid",
        },
      ],
      "@typescript-eslint/no-empty-function": "error",
      "@typescript-eslint/no-empty-interface": "error",
      "@typescript-eslint/no-explicit-any": "off",
      "@typescript-eslint/no-misused-new": "error",
      "@typescript-eslint/no-misused-promises": "off",
      "@typescript-eslint/require-await": "off",
      "@typescript-eslint/restrict-template-expressions": [
        "off",
        { allowBoolean: true },
      ],
      "@typescript-eslint/no-namespace": "error",
      "@typescript-eslint/no-parameter-properties": "off",
      "@typescript-eslint/no-shadow": [
        "error",
        {
          hoist: "all",
        },
      ],
      "@typescript-eslint/restrict-plus-operands": "off",
      "@typescript-eslint/no-unused-expressions": "error",
      "@typescript-eslint/no-use-before-define": "off",
      "no-unused-vars": "off",
      "@typescript-eslint/no-unused-vars": [
        "error",
        {
          argsIgnorePattern: "etc|props",
          varsIgnorePattern: "response|res",
        },
      ],
      "@typescript-eslint/prefer-for-of": "error",
      "@typescript-eslint/prefer-function-type": "error",
      "@typescript-eslint/prefer-namespace-keyword": "error",
      "@typescript-eslint/triple-slash-reference": [
        "error",
        {
          path: "always",
          types: "prefer-import",
          lib: "always",
        },
      ],
      "@typescript-eslint/typedef": "off",
      "@typescript-eslint/unified-signatures": "error",
      "jsdoc/check-alignment": "error",
      "jsdoc/check-indentation": ["error", { excludeTags: ["swagger"] }],
      "jsdoc/require-asterisk-prefix": ["error", "always"],
      "jsdoc/empty-tags": "error",
      "jsdoc/require-param-name": "error",
      "jsdoc/require-jsdoc": [
        "error",
        { require: { ClassDeclaration: true, MethodDefinition: true } },
      ],
      "no-empty": "error",
      "tailwindcss/no-custom-classname": [
        "warn",
        {
          "whitelist": ["(?!(bg|text)\\-).*", "(?!(.*)\\:).*"]
        }
      ],
    },
  }
];

export default eslintConfig;
