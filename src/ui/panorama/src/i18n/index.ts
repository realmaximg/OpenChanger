import { en } from './en';
import { ru } from './ru';

export const t = $.Language() === 'russian' ? ru : en;

export function format(text: string, values: Record<string, string | number>) {
  return text.replace(/\{(\w+)\}/g, (_, name: string) => String(values[name] ?? ''));
}
