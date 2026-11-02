export type MenuItem = { label: string; current?: boolean; on: () => void };

let anchors = 0;

export function anchorId() {
  anchors += 1;
  return `OpenChangerAnchor${anchors}`;
}

export function showMenu(anchor: string, items: MenuItem[]) {
  const menu = UiToolkitAPI.ShowSimpleContextMenu(
    anchor,
    'OpenChangerMenu',
    items.map((item) => ({ label: `${item.current ? '✓' : '  '}  ${item.label}`, jsCallback: item.on })),
  );
  const body = menu.FindChildTraverse('ContextMenuBody');
  if (body) {
    body.style.maxHeight = '600px';
    body.style.overflow = 'squish scroll';
  }
}
