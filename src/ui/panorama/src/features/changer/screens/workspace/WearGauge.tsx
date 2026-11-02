import { createEffect, createSignal, For, Show } from 'solid-js';
import { FieldLabel } from '../../../../components/FieldLabel/FieldLabel';
import { NumberField } from '../../../../components/NumberField/NumberField';
import { SliderField } from '../../../../components/SliderField/SliderField';
import { tokens } from '../../../../design/tokens';
import { t } from '../../../../i18n';
import { type Band, bandOf } from '../../wear';
import css from './WearGauge.module.css';

type WearGaugeProps = {
  value: number;
  min: number;
  max: number;
  decimals: number;
  bands?: Band[];
  onCommit: (value: number) => void;
};

type Zone = { band: Band; shade: string; from: number; to: number };

const shades = [tokens.color.band0, tokens.color.band1, tokens.color.band2, tokens.color.band3, tokens.color.band4];
const whole: Band = { from: 0, to: 1, label: '', name: '' };

export function WearGauge(props: WearGaugeProps) {
  const [live, setLive] = createSignal(props.value);
  createEffect(() => setLive(props.value));
  const clamp = (value: number) => Math.min(Math.max(Number(value.toFixed(props.decimals)), props.min), props.max);
  const commit = (value: number) => {
    setLive(clamp(value));
    props.onCommit(clamp(value));
  };
  const zones = (): Zone[] =>
    (props.bands ?? [whole])
      .map((band, index) => ({
        band,
        shade: shades[index],
        from: Math.max(band.from, props.min),
        to: Math.min(band.to, props.max),
      }))
      .filter((zone) => zone.to > zone.from);
  const share = (zone: Zone) => (zone.to - zone.from) / (props.max - props.min || 1);
  const width = (zone: Zone) => `${(share(zone) * 100).toFixed(2)}%`;
  return (
    <Panel class={css.gauge}>
      <Panel class={css.top}>
        <Panel class={css.readout}>
          <FieldLabel inline text={t.wear} />
        </Panel>
        <NumberField
          class={css.field}
          value={live()}
          min={props.min}
          max={props.max}
          decimals={props.decimals}
          onChange={commit}
        />
      </Panel>
      <Show when={props.bands}>
        <Panel class={css.labels}>
          <For each={zones()}>
            {(zone) => (
              <Button
                class={css.label}
                classList={{ [css.current]: bandOf(live()) === zone.band }}
                style={{ width: width(zone) }}
                tabindex="auto"
                selectionpos="auto"
                onactivate={() => commit((zone.from + zone.to) / 2)}
              >
                <Label class={css.short} text={share(zone) < 0.05 ? '' : zone.band.label} />
              </Button>
            )}
          </For>
        </Panel>
      </Show>
      <Panel class={css.ruler}>
        <Panel class={css.bar} hittest={false}>
          <For each={zones()}>
            {(zone) => <Panel class={css.segment} style={{ width: width(zone), backgroundColor: zone.shade }} />}
          </For>
        </Panel>
        <SliderField
          min={props.min}
          max={props.max}
          step={1 / 10 ** props.decimals}
          value={live()}
          onChange={(value) => setLive(clamp(value))}
          onRelease={commit}
        />
      </Panel>
    </Panel>
  );
}
