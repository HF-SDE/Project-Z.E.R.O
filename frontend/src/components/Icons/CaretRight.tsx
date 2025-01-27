import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "chevron-right"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns chevron-right icon
 */
export function CaretRight(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M9 5l7 7-7 7" />
            </svg>
        </div>
    )
}