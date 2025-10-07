import { createRouter, createWebHistory } from 'vue-router';

// import MainPage from "./components/MainPage.vue";
// import App from "@/App.vue";
import MainPage from "@/components/MainPage.vue";
import Settings from  "@/components/Settings.vue";
// import SettingsTema from "@/components/SettingsTema.vue";
import SettingsUpdate from "@/components/SettingsUpdate.vue";
import SettingsTema from "@/components/SettingsTema.vue";

const routes = [
    {
        path: '/',
        name: 'Home',
        component: MainPage
    },
    {
        path: '/settings',
        name: 'Settings',
        component: Settings
    },
    {
        path: '/settings', // Rute untuk halaman pengaturan utama
        name: 'Settings',
        children: [
            {
                path: 'update', // Rute untuk halaman pembaruan
                name: 'UpdateSettings',
                component: SettingsUpdate
            },
            {
                path: '', // Rute untuk halaman tema (jika tidak ada sub-path)
                name: 'Settings',
                component: SettingsTema
            },
            {
                path: 'tema', // Rute untuk halaman tema
                name: 'ThemeSettings',
                component: SettingsTema
            },

        ]
    }
];

const router = createRouter({
    history: createWebHistory(),
    routes
});

export default router;
