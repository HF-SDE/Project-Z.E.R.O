import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "bars-3-bottom-left"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns bars-3-bottom-left icon
 */
export function MenuIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M4 6h16M4 12h16M4 18h7" />
            </svg>
        </div>
    )
}