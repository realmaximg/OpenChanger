import { getOwner, onCleanup } from 'solid-js';
import { createRenderer } from 'solid-js/universal';
import { guarded } from '../bridge/log';
import { animationEnded, isLeaving, leave } from '../design/motion';

type Props = Record<string, unknown>;
type Handler = (panel: GenericPanel, ...args: unknown[]) => void;

const textNodes = new WeakSet<GenericPanel>();
const routed = new Map<string, WeakMap<GenericPanel, Handler>>();
const disposed = new WeakSet<GenericPanel>();
const focusing = new Set<GenericPanel>();
let holder: Panel;
let root: GenericPanel;

function setClasses(panel: GenericPanel, next: string, previous: string) {
  const wanted = next.split(' ').filter(Boolean);
  for (const name of previous.split(' ')) {
    if (name && !wanted.includes(name)) panel.RemoveClass(name);
  }
  for (const name of wanted) panel.AddClass(name);
}

function setClassList(panel: GenericPanel, next: Record<string, boolean>, previous: Record<string, boolean> = {}) {
  for (const name of Object.keys(previous)) {
    if (!(name in next)) panel.RemoveClass(name);
  }
  for (const [name, on] of Object.entries(next)) panel.SetHasClass(name, on);
}

function setStyle(panel: GenericPanel, next: Props, previous: Props = {}) {
  const style = panel.style as unknown as Record<string, unknown>;
  for (const name of Object.keys(previous)) {
    if (!(name in next)) style[name] = null;
  }
  for (const [name, value] of Object.entries(next)) style[name] = value ?? null;
}

function setRouted(panel: GenericPanel, event: string, handler: Handler | undefined) {
  let handlers = routed.get(event);
  if (!handlers) {
    const created = new WeakMap<GenericPanel, Handler>();
    const dispatch = (target: GenericPanel, ...args: unknown[]) => created.get(target)?.(target, ...args);
    $.RegisterEventHandler(event as 'AnimationEnd', root, guarded(dispatch));
    routed.set(event, created);
    handlers = created;
  }
  if (handler) handlers.set(panel, handler);
  else handlers.delete(panel);
}

function setEvent(panel: GenericPanel, event: string, handler: Handler | undefined) {
  if (!handler) return panel.ClearPanelEvent(event);
  const run = guarded(() => handler(panel));
  panel.SetPanelEvent(event, run);
}

function setTooltip(panel: GenericPanel, text: string | undefined) {
  setEvent(panel, 'onmouseover', text ? () => UiToolkitAPI.ShowTextTooltipOnPanel(panel, text) : undefined);
  setEvent(panel, 'onmouseout', text ? () => UiToolkitAPI.HideTextTooltip() : undefined);
}

function attached(panel: GenericPanel) {
  for (let current: GenericPanel | null = panel; current; current = current.GetParent()) {
    if (current === holder) return false;
    if (current === root) return true;
  }
  return false;
}

function focusAttached() {
  for (const panel of focusing) {
    if (!panel.IsValid()) focusing.delete(panel);
    else if (attached(panel)) {
      focusing.delete(panel);
      panel.SetFocus();
    }
  }
}

function setFocus(panel: GenericPanel, on: boolean) {
  if (!on) return focusing.delete(panel);
  focusing.add(panel);
  focusAttached();
}

function owned<Created extends GenericPanel>(panel: Created) {
  if (!getOwner()) return panel;
  onCleanup(() => {
    disposed.add(panel);
    if (panel.IsValid() && panel.GetParent() === holder) panel.DeleteAsync(0);
  });
  return panel;
}

function setField(panel: GenericPanel, name: string, value: unknown) {
  const fields = panel as unknown as Props;
  if (!(name in panel)) panel.SetAttributeString(name, String(value));
  else if (fields[name] !== value) fields[name] = value;
}

function setProperty(panel: GenericPanel, name: string, value: unknown, previous?: unknown) {
  if (/^on[A-Z]/.test(name)) return setRouted(panel, name.slice(2), value as Handler | undefined);
  if (name.startsWith('on')) return setEvent(panel, name, value as Handler | undefined);
  switch (name) {
    case 'class':
      return setClasses(panel, String(value ?? ''), String(previous ?? ''));
    case 'classList':
      return setClassList(panel, value as Record<string, boolean>, previous as Record<string, boolean>);
    case 'style':
      return setStyle(panel, value as Props, previous as Props);
    case 'tooltip':
      return setTooltip(panel, value as string | undefined);
    case 'focus':
      return setFocus(panel, value === true);
    case 'src':
      return (panel as Image).SetImage(String(value ?? ''));
    default:
      return setField(panel, name, value);
  }
}

function atCreation(tag: string, name: string, value: unknown) {
  return typeof value === 'string' && name !== 'tooltip' && !(tag === 'TextEntry' && name === 'text');
}

function childFrom(parent: GenericPanel, index: number) {
  for (let i = index; i < parent.GetChildCount(); i++) {
    const child = parent.GetChild(i);
    if (child && !isLeaving(child)) return child;
  }
  return undefined;
}

const solid = createRenderer<GenericPanel>({
  createElement(tag: string, props: Props = {}, parent?: GenericPanel | null) {
    const { type, ...rest } = props;
    const creation: Props = {};
    for (const [name, value] of Object.entries(rest)) {
      if (atCreation(tag, name, value)) creation[name] = value;
    }
    const panelType = (tag === 'GenericPanel' ? type : tag) as keyof PanelTagNameMap;
    const panel = $.CreatePanel(panelType, parent ?? holder, String(rest.id ?? ''), creation);
    for (const [name, value] of Object.entries(rest)) {
      if (!(name in creation) && name !== 'id') setProperty(panel, name, value);
    }
    return owned(panel);
  },
  createTextNode(value: string) {
    const label = $.CreatePanel('Label', holder, '', { text: value });
    label.visible = value !== '';
    textNodes.add(label);
    return owned(label);
  },
  replaceText(node, value) {
    (node as Label).text = value;
    node.visible = value !== '';
  },
  isTextNode(node) {
    return textNodes.has(node);
  },
  insertNode(parent, node, anchor) {
    node.SetParent(parent);
    if (anchor) parent.MoveChildBefore(node, anchor);
    else if (parent.GetChildIndex(node) !== parent.GetChildCount() - 1) {
      parent.MoveChildAfter(node, parent.GetChild(parent.GetChildCount() - 1));
    }
    if (focusing.size > 0) focusAttached();
  },
  removeNode(_parent, node) {
    if (!disposed.has(node)) return node.SetParent(holder);
    if (leave(node)) return;
    node.SetParent(holder);
    node.DeleteAsync(0);
  },
  setProperty,
  getParentNode(node) {
    return node.GetParent() ?? undefined;
  },
  getFirstChild(node) {
    return childFrom(node, 0);
  },
  getNextSibling(node) {
    const parent = node.GetParent();
    return parent ? childFrom(parent, parent.GetChildIndex(node) + 1) : undefined;
  },
});

export const {
  effect,
  memo,
  createComponent,
  createElement,
  createTextNode,
  insertNode,
  insert,
  spread,
  setProp,
  mergeProps,
  use,
} = solid;

export function Layer(props: { children: JSX.Element }) {
  const layer = $.CreatePanel('Panel', root, '');
  const height = root.actuallayoutheight / (root.actualuiscale_y || 1);
  layer.style.width = '100%';
  layer.style.height = `${height}px`;
  layer.style.marginTop = `-${height}px`;
  solid.insert(layer, () => props.children);
  onCleanup(() => layer.DeleteAsync(0));
  return undefined;
}

export function render(code: () => JSX.Element, target: GenericPanel) {
  root = target;
  holder = $.CreatePanel('Panel', root, '');
  holder.visible = false;
  const view = $.CreatePanel('Panel', root, '');
  view.style.width = '100%';
  view.style.height = '100%';
  $.RegisterEventHandler('AnimationEnd', root, guarded(animationEnded));
  const dispose = solid.render(code as () => GenericPanel, view);
  return () => {
    dispose();
    root.RemoveAndDeleteChildren();
  };
}
