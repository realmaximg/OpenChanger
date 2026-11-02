import { createSignal } from 'solid-js';

type Message = { text: string; tone: 'ok' | 'bad' };

const [message, setMessage] = createSignal<Message>();

export { message };

export function toast(text: string, tone: Message['tone'] = 'ok') {
  setMessage({ text, tone });
}
