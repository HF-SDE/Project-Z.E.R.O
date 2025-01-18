import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "magnifying-glass"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns magnifying-glass icon
 */
export function SearchIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M21 21l-6-6m2-5a7 7 0 11-14 0 7 7 0 0114 0z" />
            </svg>
        </div>
    )
}