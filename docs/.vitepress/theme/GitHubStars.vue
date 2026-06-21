<script setup lang="ts">
import { ref, onMounted } from 'vue'

const url = 'https://github.com/shlomnissan/vglx'
const stars = ref<string | null>(null)

onMounted(async () => {
  try {
    const res = await fetch('https://api.github.com/repos/shlomnissan/vglx')
    if (!res.ok) return
    const { stargazers_count: n } = await res.json()
    stars.value = n >= 1000 ? (n / 1000).toFixed(1).replace(/\.0$/, '') + 'k' : `${n}`
  } catch {
    // Ignore — the button still links to GitHub.
  }
})
</script>

<template>
  <div class="github-stars">
    <a class="btn" :href="url" target="_blank" rel="noopener">
      <svg class="icon" viewBox="0 0 16 16" width="15" height="15" aria-hidden="true">
        <path fill="currentColor" d="M8 .25a.75.75 0 0 1 .673.418l1.882 3.815 4.21.612a.75.75 0 0 1 .416 1.279l-3.046 2.97.719 4.192a.75.75 0 0 1-1.088.791L8 12.347l-3.766 1.98a.75.75 0 0 1-1.088-.79l.72-4.194L.818 6.374a.75.75 0 0 1 .416-1.28l4.21-.611L7.327.668A.75.75 0 0 1 8 .25Z" />
      </svg>
      Star
    </a>
    <a v-if="stars" class="count" :href="`${url}/stargazers`" target="_blank" rel="noopener" :aria-label="`${stars} stargazers`">{{ stars }}</a>
  </div>
</template>

<style scoped>
.github-stars {
  display: inline-flex;
  height: 32px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 8px;
  font-size: 12px;
  font-weight: 500;
}

.btn,
.count {
  display: inline-flex;
  align-items: center;
  padding: 0 12px;
  color: var(--vp-c-text-1);
}

.btn { gap: 6px; }
.icon { color: #e3b341; }

.count {
  border-left: 1px solid var(--vp-c-divider);
  color: var(--vp-c-text-2);
  font-variant-numeric: tabular-nums;
  transition: color 0.25s;
}

.count:hover { color: var(--vp-c-brand-1); }

@media (max-width: 768px) {
  .github-stars { display: none; }
}
</style>
