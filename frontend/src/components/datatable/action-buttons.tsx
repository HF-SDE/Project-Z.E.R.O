import { Check, Pencil, Save, Undo, X } from "lucide-react";

export type ButtonActions = {
  edit: () => void;
  delete: () => void;
  save: () => void;
  cancel: () => void;
};

interface ActionButtonsProps {
  data: { id: string; active: boolean };
  buttonActions?: ButtonActions;
  editMode?: boolean;
  deletePermission?: boolean;
  updatePermission?: boolean;
}

export function ActionButtons({
  data,
  buttonActions,
  editMode,
  deletePermission,
  updatePermission,
}: ActionButtonsProps) {
  return (
    <div className="flex gap-4">
      {!editMode ? (
        <>
          {updatePermission && (
            <button className="btn" onClick={buttonActions?.edit}>
              <Pencil />
            </button>
          )}

          {deletePermission && (
            <button className="btn" onClick={buttonActions?.delete}>
              {data.active ? <X /> : <Check />}
            </button>
          )}
        </>
      ) : (
        <>
          <button className="btn" onClick={buttonActions?.save}>
            <Save />
          </button>
          <button className="btn" onClick={buttonActions?.cancel}>
            <Undo />
          </button>
        </>
      )}
    </div>
  );
}
