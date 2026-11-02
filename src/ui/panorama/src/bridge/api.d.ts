import type { OpenChangerMethods } from '../../generated/api';

declare global {
  type OpenChangerAPI = Omit<OpenChangerMethods, 'dev'> & {
    root: Panel;
    dev?: OpenChangerMethods['dev'];
  };

  const OpenChangerAPI: OpenChangerAPI;
}
