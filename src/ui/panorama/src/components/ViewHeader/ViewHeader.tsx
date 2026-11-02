import css from './ViewHeader.module.css';

type ViewHeaderProps = {
  title: string;
  start?: JSX.Element;
  children?: JSX.Element;
};

export function ViewHeader(props: ViewHeaderProps) {
  return (
    <Panel class={css.head}>
      {props.start}
      <Label class={css.title} text={props.title} />
      <Panel class={css.end}>{props.children}</Panel>
    </Panel>
  );
}
