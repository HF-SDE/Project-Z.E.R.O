import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "ellipsis-horizontal"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns ellipsis-horizontal icon
 */
export function DotsIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M5 12h.01M12 12h.01M19 12h.01M6 12a1 1 0 11-2 0 1 1 0 012 0zm7 0a1 1 0 11-2 0 1 1 0 012 0zm7 0a1 1 0 11-2 0 1 1 0 012 0z" />
            </svg>
        </div>
    )
}