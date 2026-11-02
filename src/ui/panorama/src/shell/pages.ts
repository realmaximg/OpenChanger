import { type Component, createSignal } from 'solid-js';

export type Page = {
  id: string;
  title: string;
  icon: string;
  Page: Component;
  back?: () => boolean;
};

const [pages, setPages] = createSignal<Page[]>([]);
const [currentPage, showPage] = createSignal('');

export { currentPage, pages, showPage };

export function addPage(page: Page) {
  setPages([...pages(), page]);
}

export function back() {
  return (
    pages()
      .find((page) => page.id === currentPage())
      ?.back?.() === true
  );
}
