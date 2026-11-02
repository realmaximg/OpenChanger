import { createSignal, For, Show } from 'solid-js';
import { PageBoundary } from '../components/PageBoundary/PageBoundary';
import { iconPath } from '../design/icons';
import { t } from '../i18n';
import { EmptyPage } from './EmptyPage';
import { currentPage, type Page, pages, showPage } from './pages';
import css from './Shell.module.css';

const showRail = false;

const settings: Page = {
  id: 'settings',
  title: t.settings,
  icon: 'settings',
  Page: () => <EmptyPage title={t.settings} />,
};

type StripItemProps = {
  icon: string;
  label: string;
  selected?: boolean;
  onSelect: () => void;
};

function StripItem(props: StripItemProps) {
  return (
    <Button
      class={css.item}
      classList={{ [css.selected]: props.selected === true }}
      tabindex="auto"
      selectionpos="auto"
      onactivate={() => props.onSelect()}
    >
      <Image class={css.icon} src={iconPath(`ui/${props.icon}`)} />
      <Label class={css.label} text={props.label} />
    </Button>
  );
}

export function Shell(props: { onClose: () => void }) {
  const all = () => [...pages(), settings];
  showPage(all()[0].id);
  const [shown, setShown] = createSignal([currentPage()]);
  const select = (id: string) => {
    showPage(id);
    if (!shown().includes(id)) setShown([...shown(), id]);
  };
  const item = (page: Page) => (
    <StripItem
      icon={page.icon}
      label={page.title}
      selected={currentPage() === page.id}
      onSelect={() => select(page.id)}
    />
  );
  return (
    <Panel class={css.shell}>
      <Show when={showRail}>
        <Panel class={css.strip}>
          <Panel class={css.features}>
            <For each={pages()}>{item}</For>
          </Panel>
          <Panel class={css.bottom}>
            {item(settings)}
            <StripItem icon="cancel" label={t.close} onSelect={props.onClose} />
          </Panel>
        </Panel>
      </Show>
      <Panel class={css.pages}>
        <For each={all()}>
          {(page) => (
            <Show when={shown().includes(page.id)}>
              <Panel class={css.page} visible={currentPage() === page.id}>
                <PageBoundary>
                  <page.Page />
                </PageBoundary>
              </Panel>
            </Show>
          )}
        </For>
      </Panel>
    </Panel>
  );
}
