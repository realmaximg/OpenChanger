import { guarded } from './log';

type EventName = `${string}.${string}`;
type Listener = () => void;

const listeners = new Map<string, Set<Listener>>();
let subscribed = false;

function dispatch(feature: string, event: string) {
  for (const listener of [...(listeners.get(`${feature}.${event}`) ?? [])]) listener();
}

export function on(name: EventName, listener: Listener) {
  if (!subscribed) OpenChangerAPI.events.subscribe(guarded(dispatch));
  subscribed = true;
  const named = listeners.get(name) ?? new Set<Listener>();
  listeners.set(name, named.add(listener));
  return () => {
    named.delete(listener);
  };
}
