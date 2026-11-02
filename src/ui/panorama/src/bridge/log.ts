export function log(text: string) {
  if (OpenChangerAPI.dev) OpenChangerAPI.dev.log(text);
  else $.Msg(`openchanger: ${text}`);
}

export function report(error: unknown) {
  log(error instanceof Error ? `${error.message}\n${error.stack ?? ''}` : String(error));
}

export function guarded<Args extends unknown[], Result>(body: (...args: Args) => Result) {
  return (...args: Args) => {
    try {
      return body(...args);
    } catch (error) {
      report(error);
      return undefined;
    }
  };
}

export function probeStyles(root: GenericPanel) {
  if (!OpenChangerAPI.dev) return;
  const probe = $.CreatePanel('Panel', root, '', { class: 'oc-styles-end' });
  $.RegisterEventHandler('AnimationStart', probe, () => {
    log('styles parsed to the end');
    probe.DeleteAsync(0);
  });
}
