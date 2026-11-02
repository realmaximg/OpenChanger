import { batch, createComputed, createMemo, createSignal, For, on } from 'solid-js';
import css from './WindowedGrid.module.css';

type WindowedGridProps<Item> = {
  items: Item[];
  chunk: number;
  chunkHeight: number;
  class?: string;
  children: (item: Item) => JSX.Element;
};

const chunks = 5;

export function WindowedGrid<Item>(props: WindowedGridProps<Item>) {
  const [start, setStart] = createSignal(0);
  const [end, setEnd] = createSignal(0);
  let viewport: Panel | undefined;
  let stale: number | undefined;
  createComputed(
    on(
      () => props.items,
      () => {
        batch(() => {
          setStart(0);
          setEnd(props.chunk);
        });
        viewport?.ScrollToTop();
      },
    ),
  );
  const shown = createMemo(() => props.items.slice(start(), end()));
  const move = (from: number, to: number, shift: number) => {
    batch(() => {
      setStart(from);
      setEnd(to);
    });
    if (!viewport || shift === 0) return;
    stale = -viewport.scrolloffset_y;
    const top = stale + shift * viewport.actualuiscale_y;
    viewport.ScrollToFitRegion(0, 0, top, top + viewport.actuallayoutheight, 0, true, false);
  };
  const scrolled = () => {
    if (!viewport) return;
    const offset = -viewport.scrolloffset_y;
    if (offset === stale) return;
    stale = undefined;
    const margin = (props.chunkHeight / 3) * viewport.actualuiscale_y;
    const full = end() - start() >= props.chunk * chunks;
    if (offset + viewport.actuallayoutheight >= viewport.contentheight - margin && end() < props.items.length) {
      move(full ? start() + props.chunk : start(), end() + props.chunk, full ? -props.chunkHeight : 0);
    } else if (offset <= margin && start() > 0) {
      move(start() - props.chunk, full ? end() - props.chunk : end(), props.chunkHeight);
    }
  };
  return (
    <Panel
      class={css.viewport}
      ref={(panel) => {
        viewport = panel;
        panel.SetSendScrollPositionChangedEvents(true);
      }}
      onScrollPositionChanged={scrolled}
    >
      <Panel class={`${css.grid} ${props.class ?? ''}`}>
        <For each={shown()}>{props.children}</For>
      </Panel>
    </Panel>
  );
}
