import { Show } from 'solid-js';
import { triggerClass } from '../../design/motion';
import css from './ActionButton.module.css';

type ActionButtonProps = {
  label?: string;
  icon?: string;
  variant?: 'primary' | 'ghost' | 'danger' | 'installed';
  small?: boolean;
  enabled?: boolean;
  focus?: boolean;
  tooltip?: string;
  class?: string;
  onPress: () => void;
};

export function ActionButton(props: ActionButtonProps) {
  return (
    <Button
      class={`${css.button} ${props.class ?? ''}`}
      classList={{
        [css.primary]: props.variant === 'primary',
        [css.installed]: props.variant === 'installed',
        [css.ghost]: props.variant === 'ghost',
        [css.danger]: props.variant === 'danger',
        [css.small]: props.small === true,
        [css.square]: props.icon !== undefined && props.label === undefined,
        [css.withIcon]: props.icon !== undefined && props.label !== undefined,
      }}
      enabled={props.enabled ?? true}
      focus={props.focus === true}
      tabindex="auto"
      selectionpos="auto"
      tooltip={props.tooltip}
      onactivate={(panel) => {
        triggerClass(panel, 'press');
        props.onPress();
      }}
    >
      <Show when={props.icon}>{(icon: () => string) => <Image class={css.icon} src={icon()} />}</Show>
      <Show when={props.label !== undefined}>
        <Label class={css.label} text={props.label ?? ''} />
      </Show>
    </Button>
  );
}
