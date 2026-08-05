import { defineConfig } from 'vitepress'
import referenceSidebar from './auto-reference-sidebar'
import mathjax3 from 'markdown-it-mathjax3'

const year = new Date().getFullYear();

export default defineConfig({
  srcDir: 'pages',
  title: 'VGLX',
  description: '3D Rendering in Modern C++',
  cleanUrls: true,
  appearance: 'force-dark',
  markdown: {
    config: (md) => {
        md.use(mathjax3)
    }
  },
  head: [
    ['link', { rel: 'preload', href: 'logo.woff2', as: 'font', type: 'font/woff2', crossorigin: '' }],
    ['link', { rel: 'preconnect', href: 'https://fonts.googleapis.com' }],
    ['link', { rel: 'preconnect', href: 'https://fonts.gstatic.com', crossorigin: '' }],
    ['link', { rel: 'stylesheet', href: 'https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&family=JetBrains+Mono:ital,wght@0,100..800;1,100..800&display=swap' }],
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/favicon.svg' }],
    ['script', { defer: '', src: 'https://cloud.umami.is/script.js', 'data-website-id': 'aaaf1cfd-b988-46e3-9517-213509176d49'}]
  ],
  themeConfig: {
    search: {
      provider: 'local'
    },
    logo: {
      light: '/logo-light.svg',
      dark: '/logo-dark.svg'
    },
    nav: [
      { text: 'Manual', link: '/manual/', activeMatch: '^/manual/' },
      { text: 'Reference', link: '/reference/', activeMatch: '^/reference/' },
    ],
    sidebar: {
      '/manual/': [
        {
          text: 'Getting Started',
          items: [
            { text: 'Introduction', link: '/manual/' },
            { text: 'Installation', link: '/manual/installation' },
            { text: 'Creating an Application', link: '/manual/creating_application' },
            { text: 'Importing Assets', link: '/manual/importing_assets' },
            { text: 'Custom Shaders', link: '/manual/custom_shaders' }
          ]
        },
        { text: 'Reference', link: '/reference/' }
      ],
      '/reference/': referenceSidebar,
    },
    footer: {
      message: 'Released under the MIT License.',
      copyright: `Copyright © 2024–${year}`
    },
    socialLinks: [
      { icon: 'github', link: 'https://github.com/shlomnissan/vglx' }
    ]
  }
})
