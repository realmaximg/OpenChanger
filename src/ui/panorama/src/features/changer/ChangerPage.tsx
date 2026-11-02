import { t } from '../../i18n';
import type { Page } from '../../shell/pages';
import { back } from './back';
import { Changer } from './screens/Changer';

export const changerPage: Page = {
  id: 'changer',
  title: t.changer,
  icon: 'inventory',
  Page: Changer,
  back,
};
