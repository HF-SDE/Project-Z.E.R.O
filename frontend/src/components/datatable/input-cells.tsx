import { useState } from "react";

import { Input } from "@/components/ui/input";

import { Check, X } from "lucide-react";

interface InputCellsProps<TData> {
  original: TData;
  init: string | boolean;
  rowKey: keyof TData;
  setEditing: React.Dispatch<React.SetStateAction<TData[]>>;
}

export default function InputCells<TData>({
  original,
  init,
  rowKey,
  setEditing,
}: InputCellsProps<TData>) {
  const [value, setValue] = useState(init);

  function handleChange(e: React.ChangeEvent<HTMLInputElement> | boolean) {
    const newValue = typeof e === "boolean" ? e : e.target.value;

    setValue(newValue);
    setEditing((prev) => {
      const rowToEdit = prev.find((row) => row == original);
      if (rowToEdit) (rowToEdit[rowKey] as string | boolean) = newValue;

      return prev;
    });
  }

  return typeof value === "boolean" ? (
    <button
      onClick={() => handleChange(!value)}
      className="rounded-sm p-[1px] outline outline-1"
    >
      {value ? <Check /> : <X />}
    </button>
  ) : (
    <Input
      type="text"
      className="h-auto"
      value={value}
      onChange={handleChange}
      size={value.toString().length - 1}
    />
  );
}
