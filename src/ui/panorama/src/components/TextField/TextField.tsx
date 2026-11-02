import { Show } from 'solid-js';
import { icons } from '../../design/icons';
import css from './TextField.module.css';

type TextFieldProps = {
  value: string;
  placeholder?: string;
  invalid?: boolean;
  numeric?: boolean;
  number?: boolean;
  search?: boolean;
  maxchars?: number;
  class?: string;
  onChange: (text: string) => void;
  onDone?: () => void;
};

export function TextField(props: TextFieldProps) {
  return (
    <Panel
      class={`${css.field} ${props.class ?? ''}`}
      classList={{
        [css.invalid]: props.invalid === true,
        [css.number]: props.number === true,
        [css.search]: props.search === true,
      }}
    >
      <Show when={props.search}>
        <Image class={css.icon} src={icons.search} />
      </Show>
      <TextEntry
        class={css.entry}
        textmode={props.numeric ? 'numeric' : 'normal'}
        maxchars={props.maxchars}
        text={props.value}
        tabindex="auto"
        selectionpos="auto"
        onblur={() => props.onDone?.()}
        oninputsubmit={() => props.onDone?.()}
        ontextentrychange={(entry) => {
          if (entry.text !== props.value) props.onChange(entry.text);
        }}
      />
      <Label class={css.placeholder} text={props.placeholder ?? ''} visible={props.value === ''} hittest={false} />
    </Panel>
  );
}
