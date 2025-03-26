// https://nuxt.com/docs/api/configuration/nuxt-config
import Aura from '@primevue/themes/aura';

export default defineNuxtConfig({
  compatibilityDate: '2025-03-18',
  devtools: { enabled: true },
  css: ['~/assets/css/main.css'],
  modules: [
    '@primevue/nuxt-module',
    '@clerk/nuxt',
    // 'nuxt-mongoose'
  ],
  postcss: {
    plugins: {
      tailwindcss: {},
      autoprefixer: {},
    },
  },
  primevue: {
    options: {
      ripple: true,
      inputVariant: 'filled',
      theme: {
        preset: Aura,
        options: {
          prefix: 'p',
          darkModeSelector: 'system',
          cssLayer: false
        }
      }
    }
  },
  // mongoose: {
  //   uri: process.env.MONGODB_URI,
  //   options: {},
  //   modelsDir: 'models',
  //   devtools: true,
  // }
})