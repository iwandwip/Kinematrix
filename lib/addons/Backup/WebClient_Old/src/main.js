import { createApp } from 'vue'
import './index.css'
// import './style.css'

import './axiosConfig.ts';
import Vueform from '@vueform/vueform'
import vueformConfig from './../vueformConfig'
import App from './App.vue'
import router from './router';

createApp(App).use(router).use(Vueform, vueformConfig).mount('#app');
