import css from './FieldLabel.module.css';

export function FieldLabel(props: { text: string; inline?: boolean }) {
  return <Label class={css.label} classList={{ [css.inline]: props.inline === true }} text={props.text} />;
}
