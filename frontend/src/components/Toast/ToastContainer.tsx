import { useUI } from "@hooks/useUI"
import ToastMessage from "./ToastMessage";


/**
 * A container used for ToastMessage element
 * @returns
 */
export default function ToastContainer() {
    const ui = useUI();

    return (
        <div className="absolute right-0 top-0 z-[100] flex flex-col">
            {ui.popups.map(popup => (
                <ToastMessage key={popup.id} popup={popup} />
            ))}
        </div>
    )
}