// https://nuxt.com/docs/api/configuration/nuxt-config
import tailwindcss from '@tailwindcss/vite'


export default defineNuxtConfig({
  compatibilityDate: '2025-03-18',
  ssr: true,
  devtools: { enabled: true },
  css: ['~/assets/css/tailwind.css'],
  modules: [
    'shadcn-nuxt',
    '@clerk/nuxt',
    // '@nuxtjs/supabase'
  ],
  clerk: {
    skipServerMiddleware: true,
  },
  vite: {
    plugins: [
      tailwindcss(),
    ],
  },
  shadcn: {
    /**
     * Prefix for all the imported component
     */
    prefix: '',
    /**
     * Directory that the component lives in.
     * @default "./components/ui"
     */
    componentDir: './components/ui'
  },
  // supabase : {
  //   redirect: false,
  //   redirectOptions:{
  //     login: '/login',
  //     callback: '/callback',
  //     exclude: ['*']
  //   }
  // }
})