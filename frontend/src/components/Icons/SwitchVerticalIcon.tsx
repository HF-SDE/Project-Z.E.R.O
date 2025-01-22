import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "arrows-up-down"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns arrows-up-down icon
 */
export function SwitchVerticalIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M7 16V4m0 0L3 8m4-4l4 4m6 0v12m0 0l4-4m-4 4l-4-4" />
            </svg>
        </div>
    )
}