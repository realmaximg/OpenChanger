import { For } from 'solid-js';
import { tokens } from '../../design/tokens';
import css from './Spinner.module.css';

const loop = Number.parseFloat(tokens.duration.loop);
const spokes = Array.from({ length: 12 }, (_, index) => index);

export function Spinner() {
  return (
    <Panel class={css.spinner} hittest={false}>
      <For each={spokes}>
        {(index) => (
          <Panel class={css.arm} style={{ transform: `rotateZ(${index * 30}deg)` }}>
            <Panel class={css.spoke} style={{ animationDelay: `${((index * loop) / 12).toFixed(3)}s` }} />
          </Panel>
        )}
      </For>
    </Panel>
  );
}
