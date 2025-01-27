import { useCallback, useState } from "react";

/**
 * Used for loading
 * 
 * @returns 
 */
export function useButton(): [boolean, () => void] {
    const [isLoading, setIsLoading] = useState<boolean>(false);

    const toggleLoading = useCallback(() => setIsLoading(prevState => !prevState), []);

    return [isLoading, toggleLoading];
}
