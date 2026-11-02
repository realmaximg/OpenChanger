import './api-types.mjs';
import { mkdir, readFile, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { transformAsync } from '@babel/core';
import esbuild from 'esbuild';
import { transform } from 'lightningcss';
import { tokens } from './src/design/tokens.ts';

const release = process.argv.includes('--release');
const watch = process.argv.includes('--watch');
const root = import.meta.dirname;
const renderer = path.join(root, 'src/renderer/panorama.ts').replaceAll('\\', '/');
const panoramaTargets = { chrome: 50 << 16 };
const motionCss = 'src/design/motion.module.css';
const styles = new Map();

function tokenValues(group, prefix, values) {
  for (const [key, value] of Object.entries(group)) {
    const name = `${prefix}-${key}`;
    if (typeof value === 'object') tokenValues(value, name, values);
    else values.set(name, value);
  }
  return values;
}

const tokenMap = tokenValues(tokens, '-', new Map());

function withTokens(css, file) {
  return css.replace(/var\((--[\w-]+)\)/g, (_, name) => {
    if (!tokenMap.has(name)) throw new Error(`${file}: unknown token ${name}`);
    return tokenMap.get(name);
  });
}

function quoteKeyframes(css) {
  return css.replace(/@keyframes\s+([\w-]+)/g, "@keyframes '$1'");
}

function compact(css) {
  return css.replace(/\s*\n\s*/g, '');
}

function compileModule(file, source) {
  const name = path.basename(file, '.module.css');
  const result = transform({
    filename: file,
    code: Buffer.from(withTokens(source, file)),
    cssModules: { pattern: `oc-${name}-[local]` },
    targets: panoramaTargets,
  });
  const names = Object.fromEntries(Object.entries(result.exports).map(([local, value]) => [local, value.name]));
  return { css: quoteKeyframes(result.code.toString()), names };
}

function stylesEnd() {
  const keyframes = `@keyframes 'oc-styles-end' { from { opacity: 1; } to { opacity: 1; } }`;
  const rule = `.oc-styles-end { animation-name: oc-styles-end; animation-duration: ${tokens.duration.fast}; }`;
  return `${keyframes}\n${rule}\n`;
}

function bundleCss(metafile) {
  const output = metafile.outputs[Object.keys(metafile.outputs)[0]];
  const inputs = Object.keys(output.inputs);
  const parts = [];
  for (const input of [...inputs.filter((input) => input !== motionCss), motionCss]) {
    const css = styles.get(path.join(root, input));
    if (css === undefined) continue;
    parts.push(release ? css : `/* ${input} */\n${css}`);
  }
  parts.push(stylesEnd());
  const css = parts.join('\n');
  return release ? compact(css) : css;
}

const panorama = {
  name: 'panorama',
  setup(build) {
    build.onStart(() => styles.clear());
    build.onLoad({ filter: /\.tsx$/ }, async (args) => {
      const source = await readFile(args.path, 'utf8');
      const result = await transformAsync(source, {
        filename: args.path,
        babelrc: false,
        configFile: false,
        presets: ['@babel/preset-typescript', ['babel-preset-solid-panorama', { moduleName: renderer }]],
      });
      return { contents: result.code, loader: 'js' };
    });
    build.onLoad({ filter: /\.module\.css$/ }, async (args) => {
      const { css, names } = compileModule(args.path, await readFile(args.path, 'utf8'));
      styles.set(args.path, css);
      return { contents: `export default ${JSON.stringify(names)};`, loader: 'js' };
    });
    build.onEnd(async (result) => {
      if (result.errors.length > 0) return;
      await mkdir(path.join(root, 'dist'), { recursive: true });
      await writeFile(path.join(root, 'dist/bundle.css'), bundleCss(result.metafile));
      console.log(`built ${release ? 'release' : 'dev'} dist/bundle.js dist/bundle.css`);
    });
  },
};

const context = await esbuild.context({
  absWorkingDir: root,
  entryPoints: ['src/main.tsx'],
  outfile: 'dist/bundle.js',
  bundle: true,
  format: 'iife',
  target: 'es2022',
  minify: release,
  legalComments: 'none',
  metafile: true,
  logLevel: 'warning',
  plugins: [panorama],
});

if (watch) {
  await context.watch();
} else {
  const result = await context.rebuild();
  await context.dispose();
  if (result.errors.length > 0) process.exit(1);
}
