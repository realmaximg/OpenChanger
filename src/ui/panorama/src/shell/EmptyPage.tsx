import css from './EmptyPage.module.css';

export function EmptyPage(props: { title: string }) {
  return (
    <Panel class={css.page}>
      <Label class={css.title} text={props.title} />
      <Panel class={css.body} />
    </Panel>
  );
}
