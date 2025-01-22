import { IconProps } from "../../core/types/component.types";

/**
 * Can be find heroicons via Outline and the name "x-mark"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns x-mark icon
 */
export function XIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M6 18L18 6M6 6l12 12" />
            </svg>
        </div>
    )
}