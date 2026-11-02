import { type Accessor, Show } from 'solid-js';
import { Picture } from '../../../../components/Picture/Picture';
import * as game from '../../game';
import { type KitKind, kitName } from '../../kits';
import css from './KitSlot.module.css';

type KitSlotProps = {
  kind: KitKind;
  kit: number | undefined;
  active: boolean;
  picking: boolean;
  onPress: () => void;
};

export function KitSlot(props: KitSlotProps) {
  return (
    <Button
      class={css.slot}
      classList={{ [css.filled]: props.kit !== undefined, [css.active]: props.active, [css.picking]: props.picking }}
      hittestchildren={false}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => props.onPress()}
    >
      <Show when={props.kit} fallback={<Label class={css.plus} text="+" />}>
        {(kit: Accessor<number>) => (
          <>
            <Panel class={css.image}>
              <Picture id={game.kitId(props.kind, kit())} />
            </Panel>
            <Label class={css.name} text={kitName(props.kind, kit())} />
          </>
        )}
      </Show>
    </Button>
  );
}
