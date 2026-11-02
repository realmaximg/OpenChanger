import { Show } from 'solid-js';
import { icons } from '../../design/icons';
import { anchorId, type MenuItem, showMenu } from '../Menu/menu';
import css from './DropButton.module.css';

type DropButtonProps = {
  caption?: string;
  value: string;
  items: () => MenuItem[];
  active?: boolean;
  fill?: boolean;
  class?: string;
};

export function DropButton(props: DropButtonProps) {
  const id = anchorId();
  return (
    <Button
      id={id}
      class={`${css.drop} ${props.class ?? ''}`}
      classList={{ [css.active]: props.active === true, [css.fill]: props.fill === true }}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => showMenu(id, props.items())}
    >
      <Show when={props.caption}>
        <Label class={css.caption} text={props.caption ?? ''} />
      </Show>
      <Label class={css.value} text={props.value} />
      <Image class={css.chevron} src={icons.down} />
    </Button>
  );
}
