import { IconProps } from "@types";

/**
 * Can be find heroicons via Outline and the name "bolt"
 * @param {IconProps} props
 * @link https://heroicons.com/
 * @returns bolt icon
 */
export function LightningIcon(props: IconProps) {
    return (
        <div title={props.title ?? ""} className={props.className}>
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor" strokeWidth={2}>
                <path strokeLinecap="round" strokeLinejoin="round" d="M13 10V3L4 14h7v7l9-11h-7z" />
            </svg>
        </div>
    )
}