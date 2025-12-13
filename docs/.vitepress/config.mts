import { defineConfig } from 'vitepress'
import referenceSidebar from './auto-reference-sidebar'
import mathjax3 from 'markdown-it-mathjax3'

const year = new Date().getFullYear();

export default defineConfig({
  srcDir: 'pages',
  title: 'VGLX',
  description: '3D Rendering in Modern C++',
  cleanUrls: true,
  markdown: {
    config: (md) => {
        md.use(mathjax3)
    }
  },
  head: [
    ['link', { rel: 'preload', href: 'logo.woff2', as: 'font', type: 'font/woff2', crossorigin: '' }],
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/favicon.svg', media: '(prefers-color-scheme: light)' }],
    ['link', { rel: 'icon', type: 'image/svg+xml', href: '/favicon_white.svg', media: '(prefers-color-scheme: dark)' }],
    ['link', { rel: 'icon', href: '/favicon.ico', media: '(prefers-color-scheme: light)' }],
    ['link', { rel: 'icon', href: '/favicon_white.ico', media: '(prefers-color-scheme: dark)' }],
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
      { text: 'Manual', link: '/manual/' },
      { text: 'Reference', link: '/reference/core/application' },
    ],
    sidebar: {
      '/manual/': [
        {
          text: 'Getting Started',
          items: [
            { text: 'Introduction', link: '/manual/' },
            { text: 'Installation', link: '/manual/installation' },
            { text: 'Creating an Application', link: '/manual/creating_application' },
            { text: 'Importing Assets', link: '/manual/importing_assets' }
          ]
        },
        { text: 'Reference', link: '/reference/core/application' }
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
