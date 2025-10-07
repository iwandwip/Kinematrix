
import { createGlobalState } from '@vueuse/core';
import { ref } from 'vue';

// const settingStateManagement = createGlobalState(() => {
//     return useStorage('settings', {
//         themes: 'classic',
//         colors: 'dark',
//         loader: false,
//         setThemes: (newTheme: string) => {
//             themes = newTheme;
//         }
//     });
// });


// Buat state global
const settingStateManagement = createGlobalState(() => {
    // Getter
    const  themes = ref('classic');
    const  colors = ref('dark');
    const loader = ref(false);
    const page = ref('index');
    const failedLoad = ref(0);

    // Setters
    const setThemes = (newTheme : string) => {
        try {
            themes.value = newTheme;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };
    const setColors = (newColor : string) => {
        try {
            colors.value = newColor;
            // Requesst AXIOS
        }
        catch (e) {
            // e
        }
    };

    const startLoading = () => {
        loader.value = true;
    }

    const stopLoading = () => {
        loader.value = false;
    }
    return {
        themes, colors, loader, page,failedLoad,
        setColors,setThemes,
        startLoading, stopLoading
    };
});

export default settingStateManagement;
