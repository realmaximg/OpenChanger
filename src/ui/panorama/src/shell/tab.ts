import { createSignal } from 'solid-js';
import { guarded } from '../bridge/log';
import { iconPath } from '../design/icons';
import { t } from '../i18n';
import { back } from './pages';

const hiddenClass = 'mainmenu-content--hidden';
const [tabShown, setTabShown] = createSignal(false);

export { tabShown };

function find(id: string) {
  const panel = $.GetContextPanel().FindChildInLayoutFile(id);
  if (!panel) throw new Error(`the main menu has no ${id}`);
  return panel;
}

function hideWhenFaded(tab: Panel) {
  return (panel: GenericPanel, property: string) => {
    if (panel.id !== tab.id || property !== 'opacity') return false;
    if (tab.visible && tab.BIsTransparent()) {
      tab.SetReadyForDisplay(false);
      tab.visible = false;
      setTabShown(false);
      return true;
    }
    if (tab.visible) {
      $.DispatchEvent('MainMenuTabShown', tab.id);
      setTabShown(true);
    }
    return false;
  };
}

function createButton(tab: Panel, settings: GenericPanel) {
  const bar = settings.GetParent() as Panel;
  const button = $.CreatePanel('RadioButton', bar, '', {
    class: 'mainmenu-top-navbar__radio-iconbtn',
    group: 'NavBar',
  });
  $.CreatePanel('Image', button, '', {
    class: 'mainmenu-top-navbar__radio-btn__icon',
    src: iconPath('ui/colorpicker'),
  });
  const events = {
    onactivate: () => MainMenu.NavigateToTab(tab.id, ''),
    onmouseover: () => UiToolkitAPI.ShowTextTooltipOnPanel(button, t.appName),
    onmouseout: () => UiToolkitAPI.HideTextTooltip(),
  };
  for (const [event, handler] of Object.entries(events)) button.SetPanelEvent(event, guarded(handler));
  bar.MoveChildAfter(button, settings);
  return button;
}

export function closeTab() {
  MainMenu.OnHomeButtonPressed();
}

export function createTab(tab: Panel) {
  const settings = find('MainMenuNavBarSettings');
  tab.SetParent(find('JsMainMenuContent'));
  tab.AddClass('mainmenu-content__container');
  tab.AddClass(hiddenClass);
  tab.hittest = true;
  tab.visible = false;
  tab.SetReadyForDisplay(false);
  tab.RegisterForReadyEvents(true);
  $.RegisterEventHandler('PropertyTransitionEnd', tab, guarded(hideWhenFaded(tab)));
  $.RegisterKeyBind(
    tab,
    'key_escape',
    guarded(() => {
      if (!back()) closeTab();
    }),
  );
  const button = createButton(tab, settings);
  return () => {
    if (!tab.BHasClass(hiddenClass)) closeTab();
    button.DeleteAsync(0);
  };
}
