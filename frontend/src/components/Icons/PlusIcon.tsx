import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "plus-circle"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns plus-circle icon
 */
export function PlusIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M12 9v3m0 0v3m0-3h3m-3 0H9m12 0a9 9 0 11-18 0 9 9 0 0118 0z" />
            </svg>
        </div>
    )
}