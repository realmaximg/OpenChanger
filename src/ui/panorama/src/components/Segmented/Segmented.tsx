import { For } from 'solid-js';
import css from './Segmented.module.css';

type Option<Value extends string> = { value: Value; label: string; color?: string };

type SegmentedProps<Value extends string> = {
  options: Option<Value>[];
  value: Value;
  onChange: (value: Value) => void;
};

export function Segmented<Value extends string>(props: SegmentedProps<Value>) {
  return (
    <Panel class={css.segmented}>
      <For each={props.options}>
        {(option) => {
          const active = () => option.value === props.value;
          return (
            <Button
              class={css.option}
              classList={{ [css.active]: active(), [css.tinted]: active() && option.color !== undefined }}
              style={{ backgroundColor: active() ? option.color : undefined }}
              tabindex="auto"
              selectionpos="auto"
              onactivate={() => props.onChange(option.value)}
            >
              <Label class={css.label} text={option.label} />
            </Button>
          );
        }}
      </For>
    </Panel>
  );
}
