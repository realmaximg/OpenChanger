import { type Accessor, For, Show } from 'solid-js';
import { ActionButton } from '../../../../components/ActionButton/ActionButton';
import { Chip } from '../../../../components/Chip/Chip';
import { DropButton } from '../../../../components/DropButton/DropButton';
import { FieldLabel } from '../../../../components/FieldLabel/FieldLabel';
import { NumberField } from '../../../../components/NumberField/NumberField';
import { Segmented } from '../../../../components/Segmented/Segmented';
import { icons } from '../../../../design/icons';
import { tokens } from '../../../../design/tokens';
import { t } from '../../../../i18n';
import type { Paint } from '../../api';
import { type Entry, souvenirEvents, variantLabel } from '../../catalog';
import { chooseVariant, draft, edit, randomSeed, seedMax, selected, souvenirAllowed } from '../../editor';
import * as game from '../../game';
import { bands } from '../../wear';
import css from './FinishTab.module.css';
import { WearGauge } from './WearGauge';

type Quality = 'normal' | 'stattrak' | 'souvenir';

const eventName = (event: number) => game.localized(`#CSGO_Tournament_Event_Name_${event}`, String(event));

function qualityOf(): Quality {
  const record = draft();
  return record?.StatTrak ? 'stattrak' : record?.SouvenirEvent ? 'souvenir' : 'normal';
}

function setQuality(quality: Quality) {
  edit({ StatTrak: quality === 'stattrak', SouvenirEvent: quality === 'souvenir' ? (souvenirEvents()[0] ?? 0) : 0 });
}

function Variants(props: { entry: Entry }) {
  return (
    <Panel class={css.section}>
      <FieldLabel text={t.variant} />
      <Panel class={css.wrap}>
        <For each={props.entry.variants}>
          {(variant) => (
            <Chip
              label={variant.paint ? variantLabel(variant.paint) : variant.title}
              active={selected()?.key === variant.key}
              onPress={() => chooseVariant(variant)}
            />
          )}
        </For>
      </Panel>
    </Panel>
  );
}

function Pattern() {
  return (
    <Panel class={css.group}>
      <FieldLabel text={t.pattern} />
      <Panel class={css.fields}>
        <NumberField
          class={css.seed}
          value={draft()?.Seed ?? 0}
          min={0}
          max={seedMax}
          onChange={(value) => edit({ Seed: value })}
        />
        <ActionButton
          class={css.dice}
          icon={icons.random}
          tooltip={t.randomPattern}
          onPress={() => edit({ Seed: randomSeed() })}
        />
      </Panel>
    </Panel>
  );
}

function ModelPicker() {
  return (
    <Panel class={css.group}>
      <FieldLabel text={t.model} />
      <Segmented
        options={[
          { value: 'current', label: 'CS2' },
          { value: 'legacy', label: 'CS:GO' },
        ]}
        value={draft()?.Legacy ? 'legacy' : 'current'}
        onChange={(value) => edit({ Legacy: value === 'legacy' })}
      />
    </Panel>
  );
}

function QualityPicker(props: { entry: Entry; options: { value: Quality; label: string; color?: string }[] }) {
  return (
    <Panel class={css.group}>
      <FieldLabel text={t.quality} />
      <Panel class={css.fields}>
        <Segmented options={props.options} value={qualityOf()} onChange={setQuality} />
        <Show when={qualityOf() === 'stattrak'}>
          <Label class={css.kills} text={props.entry.kind === 'music' ? t.mvps : t.kills} />
          <NumberField
            class={css.count}
            value={draft()?.StatTrakCount ?? 0}
            min={0}
            max={99999}
            onChange={(value) => edit({ StatTrakCount: value })}
          />
        </Show>
      </Panel>
      <Show when={props.options.some((each) => each.value === 'souvenir') && souvenirEvents().length > 1}>
        <Panel
          class={css.eventRow}
          classList={{ [css.concealed]: qualityOf() !== 'souvenir' }}
          hittestchildren={qualityOf() === 'souvenir'}
        >
          <DropButton
            class={css.event}
            caption={t.event}
            value={eventName(draft()?.SouvenirEvent ?? 0)}
            items={() =>
              souvenirEvents().map((event) => ({
                label: eventName(event),
                current: draft()?.SouvenirEvent === event,
                on: () => edit({ SouvenirEvent: event }),
              }))
            }
          />
        </Panel>
      </Show>
    </Panel>
  );
}

export function FinishTab(props: { entry: Entry }) {
  const options = () => [
    { value: 'normal' as const, label: t.normal },
    ...(props.entry.item.StatTrak
      ? [{ value: 'stattrak' as const, label: 'StatTrak™', color: tokens.color.statTrak }]
      : []),
    ...(souvenirAllowed(props.entry) && souvenirEvents().length > 0
      ? [{ value: 'souvenir' as const, label: t.souvenir, color: tokens.color.souvenir }]
      : []),
  ];
  return (
    <>
      <Show when={props.entry.variants.length > 1}>
        <Variants entry={props.entry} />
      </Show>
      <Show when={props.entry.paint}>
        {(paint: Accessor<Paint>) => (
          <WearGauge
            value={draft()?.Wear ?? 0}
            min={paint().WearMin}
            max={paint().WearMax}
            decimals={4}
            bands={bands}
            onCommit={(value) => edit({ Wear: value })}
          />
        )}
      </Show>
      <Panel class={css.row}>
        <Show when={props.entry.paint}>
          <Pattern />
        </Show>
        <Show when={options().length > 1}>
          <QualityPicker entry={props.entry} options={options()} />
        </Show>
      </Panel>
      <Show when={props.entry.paint === undefined && props.entry.item.Legacy}>
        <ModelPicker />
      </Show>
    </>
  );
}
