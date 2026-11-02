import { createSignal } from 'solid-js';

const wideFrom = 1728;

const [wide, setWide] = createSignal(false);

export { wide };

export const columns = () => (wide() ? 4 : 3);
export const kitColumns = () => (wide() ? 5 : 4);

export function measureLayout() {
  const screen = $.GetContextPanel();
  setWide(screen.actuallayoutwidth / (screen.actualuiscale_x || 1) >= wideFrom);
}
