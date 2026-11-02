import { type Accessor, Show } from 'solid-js';
import { FieldLabel } from '../../../../components/FieldLabel/FieldLabel';
import { NumberField } from '../../../../components/NumberField/NumberField';
import { t } from '../../../../i18n';
import type { Charm } from '../../api';
import { chooseSlot, draft, edit, setCharm, slot } from '../../editor';
import { openKits, pickingKits } from '../../kits';
import css from './CharmTab.module.css';
import { KitSlot } from './KitSlot';
import { KitHead } from './KitSlotsTab';

export function CharmTab() {
  const picking = () => pickingKits('keychain');
  const charm = () => draft()?.Charm;
  return (
    <Panel class={css.layout}>
      <KitSlot
        kind="keychain"
        kit={charm()?.Kit}
        active={slot() === 0}
        picking={picking() && slot() === 0}
        onPress={() => {
          chooseSlot(0);
          openKits('keychain');
        }}
      />
      <Panel class={css.detail}>
        <Show when={slot() === 0 ? charm() : undefined}>
          {(current: Accessor<Charm>) => (
            <>
              <KitHead kind="keychain" picking={picking()} onRemove={() => setCharm(undefined)} />
              <Show when={!picking()}>
                <FieldLabel text={t.pattern} />
                <NumberField
                  class={css.field}
                  value={current().Pattern}
                  min={0}
                  max={4294967295}
                  onChange={(value) => edit({ Charm: { ...current(), Pattern: value } })}
                />
              </Show>
            </>
          )}
        </Show>
      </Panel>
    </Panel>
  );
}
