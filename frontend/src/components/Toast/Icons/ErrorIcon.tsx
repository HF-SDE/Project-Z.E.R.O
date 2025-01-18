import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "exclamation-circle"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns exclamation-circle icon
 */
export function ErrorIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M12 8v4m0 4h.01M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
        </div>
    )
}