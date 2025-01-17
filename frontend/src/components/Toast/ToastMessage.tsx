import { CheckCircleIcon, ExclamationCircleIcon, XCircleIcon, XIcon } from "@components/Icons";
import clsx from "clsx";
import { Popup, uiSlice } from "core/redux/reducers/ui.reducer";
import { useEffect } from "react";
import { useDispatch } from "react-redux";

interface Props {
    popup: Popup
}

const variants = {
    default: `
    bg-cyan-100
    text-blue-500
    border-blue-400
    `,
    success: `
    bg-green-100
    text-gray-600
    border-green-600
    `,
    error: `
    bg-red-100
    text-gray-600
    border-red-600
    `
}

const icons = {
    default: <ExclamationCircleIcon className="mr-2 size-5" />,
    success: <CheckCircleIcon className="mr-2 size-5 text-green-600" />,
    error: <XCircleIcon className="mr-2 size-5 text-red-600" />
}


export type ToastMessageVariant = keyof typeof variants;


/**
 * A Thoast Message
 * @param {Popup} popup
 * @returns A Thoast Message
 */
export default function ToastMessage(props: Props) {
    const dispatch = useDispatch();

    useEffect(() => {
        setTimeout(() => {
            removePopup();
        }, props.popup.lifeSpanMs);
    }, [props.popup.lifeSpanMs]);

    
    /**
     * Used for remove the popup from the screen
     */
    function removePopup() {
        dispatch(uiSlice.actions.REMOVE_POPUP({ popup: props.popup }));
    }

    return (
        <div className={clsx(
            variants[props.popup.type || 'default'],
            "mr-4 mt-4 flex min-w-96 flex-row rounded-md border border-solid p-3 text-sm"
        )}>
            <div className="mr-1">
                {icons[props.popup.type || 'default']}
            </div>
            <div className="flex-1">
                <p className="font-bold">{props.popup.title}</p>
                <p>{props.popup.text}</p>
            </div>
            <div>
                <button className="ml-2" onClick={() => removePopup()}>
                    <XIcon className="size-5" />
                </button>
            </div>
        </div>
    )
}