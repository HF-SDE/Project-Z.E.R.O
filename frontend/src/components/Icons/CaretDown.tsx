import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "chevron-down"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns chevron-down icon
 */
export function CaretDown(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 9l-7 7-7-7" />
            </svg>
        </div>
    )
}