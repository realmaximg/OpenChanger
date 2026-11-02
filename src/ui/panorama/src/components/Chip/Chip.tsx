import css from './Chip.module.css';

type ChipProps = {
  label: string;
  active: boolean;
  onPress: () => void;
};

export function Chip(props: ChipProps) {
  return (
    <Button
      class={css.chip}
      classList={{ [css.active]: props.active }}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => props.onPress()}
    >
      <Label class={css.label} text={props.label} />
    </Button>
  );
}
