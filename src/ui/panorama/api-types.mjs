import { existsSync } from 'node:fs';

if (!existsSync(new URL('generated/api.d.ts', import.meta.url))) {
  console.error('generated/api.d.ts is missing: build the dev DLL first (tools\\build.cmd dev prints it)');
  process.exit(1);
}
