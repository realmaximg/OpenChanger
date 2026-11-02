import { createEffect, createSignal, untrack } from 'solid-js';
import { TextField } from '../TextField/TextField';

type NumberFieldProps = {
  value: number;
  min: number;
  max: number;
  decimals?: number;
  class?: string;
  onChange: (value: number) => void;
};

export function NumberField(props: NumberFieldProps) {
  const show = (value: number) => (props.decimals ? value.toFixed(props.decimals) : String(value));
  const [text, setText] = createSignal(show(props.value));
  const read = (value: string) => {
    const number = Number(value.replace(',', '.'));
    if (value.trim() === '' || !Number.isFinite(number)) return undefined;
    return props.decimals ? number : Math.round(number);
  };
  const clamp = (number: number) => Math.min(Math.max(number, props.min), props.max);
  const inRange = (number: number | undefined) =>
    number !== undefined && number >= props.min - 1e-6 && number <= props.max + 1e-6;
  createEffect(() => {
    const value = props.value;
    if (read(untrack(text)) !== value) setText(show(value));
  });
  const done = () => {
    const number = read(text());
    const value = number === undefined ? props.value : clamp(number);
    setText(show(value));
    if (value !== props.value) props.onChange(value);
  };
  return (
    <TextField
      class={props.class}
      number
      numeric={!props.decimals}
      value={text()}
      invalid={!inRange(read(text()))}
      onChange={(next) => {
        setText(next);
        const number = read(next);
        if (number !== undefined && inRange(number)) props.onChange(clamp(number));
      }}
      onDone={done}
    />
  );
}
