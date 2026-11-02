import { ErrorBoundary } from 'solid-js';
import { report } from '../../bridge/log';
import { t } from '../../i18n';
import { ActionButton } from '../ActionButton/ActionButton';
import css from './PageBoundary.module.css';

function PageFailed(props: { error: unknown; reset: () => void }) {
  report(props.error);
  return (
    <Panel class={css.failed}>
      <Label class={css.title} text={t.pageFailed} />
      <Label class={css.hint} text={t.pageFailedHint} />
      <ActionButton class={css.reload} label={t.reloadPage} focus onPress={() => props.reset()} />
    </Panel>
  );
}

export function PageBoundary(props: { children: JSX.Element }) {
  return (
    <ErrorBoundary fallback={(error: unknown, reset: () => void) => <PageFailed error={error} reset={reset} />}>
      {props.children}
    </ErrorBoundary>
  );
}
