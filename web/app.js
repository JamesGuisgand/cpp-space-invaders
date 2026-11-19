import {renderer} from './render.js';

const $ = id => document.getElementById(id);
const draw = renderer($('field'));
const modes = ['classic','enhanced'];
const keys = new Set();
const touches = new Map();
const movement = ['ArrowLeft','ArrowRight','ArrowUp','ArrowDown','z','q','s','d','Space'];
function load(key) {
  try { return localStorage.getItem(key); } catch { return null; }
}
function write(key,value) {
  try { localStorage.setItem(key,value); } catch { /* Private or restricted storage. */ }
}
let mode = load('invaders.mode');
if (!modes.includes(mode)) mode = 'enhanced';
let best = Number(load('invaders.best'));
if (!Number.isSafeInteger(best) || best < 0) best = 0;
let engine, state = 'loading', pending = 0, previous;
const step = 1/120;

function selectMode(value) {
  mode = value;
  document.body.dataset.mode = value;
  document.querySelector('input[value="'+value+'"]').checked = true;
  write('invaders.mode',value);
}
selectMode(mode);
$('best').textContent = best;
$('graphics').addEventListener('change', event => {
  if (modes.includes(event.target.value)) selectMode(event.target.value);
});
function screen(next, title, message, action) {
  state = next;
  keys.clear();
  touches.clear();
  pending = 0;
  const playing = next === 'playing';
  $('menu').hidden = playing;
  $('pause').hidden = !playing;
  $('hud').hidden = next !== 'playing' && next !== 'paused';
  $('title').textContent = title;
  $('status').textContent = message;
  $('play').firstChild.nodeValue = action+' ';
  if (!playing && engine) $('play').focus();
}
function updateHud() {
  const score = engine._inv_score();
  $('score').textContent = score;
  $('wave').textContent = engine._inv_wave();
  $('lives').textContent = engine._inv_lives();
  if (score > best) {
    best = score;
    $('best').textContent = best;
    write('invaders.best',best);
  }
}
function play() {
  if (!engine || state === 'playing') return;
  if (state !== 'paused') engine._inv_reset(crypto.getRandomValues(new Uint32Array(1))[0]);
  screen('playing','','','Jouer');
  updateHud();
}
function pause() {
  if (state === 'playing') screen('paused','Pause','Tu peux changer le rendu puis reprendre.','Reprendre');
}
$('play').onclick = play;
$('pause').onclick = pause;
window.addEventListener('blur',pause);
document.addEventListener('visibilitychange',() => { if (document.hidden) pause(); });
const keyName = event => event.code === 'Space' ? 'Space' : event.key.toLowerCase().length === 1 ? event.key.toLowerCase() : event.key;
window.addEventListener('keydown',event => {
  if (event.key === 'Escape' && !event.repeat) {
    if (state === 'playing') pause();
    else if (state === 'paused') play();
    return;
  }
  const key = keyName(event);
  if (state === 'playing' && movement.includes(key)) {
    event.preventDefault();
    keys.add(key);
  }
});
window.addEventListener('keyup',event => keys.delete(keyName(event)));
for (const button of document.querySelectorAll('.touch button')) {
  button.addEventListener('pointerdown',event => {
    if (state !== 'playing') return;
    event.preventDefault();
    button.setPointerCapture(event.pointerId);
    touches.set(event.pointerId,button.dataset.key);
  });
  for (const type of ['pointerup','pointercancel','lostpointercapture'])
    button.addEventListener(type,event => touches.delete(event.pointerId));
}
function tick(now) {
  const elapsed = previous === undefined ? 0 : Math.min(.1,(now-previous)/1000);
  previous = now;
  if (state === 'playing') {
    const active = key => keys.has(key) || [...touches.values()].includes(key);
    const x = Number(active('ArrowRight') || active('d')) - Number(active('ArrowLeft') || active('q'));
    const y = Number(active('ArrowDown') || active('s')) - Number(active('ArrowUp') || active('z'));
    const fire = Number(active('Space'));
    pending += elapsed;
    let updated = false;
    while (pending >= step && engine._inv_lives() > 0) {
      engine._inv_step(x,y,fire);
      pending -= step;
      updated = true;
    }
    if (updated) updateHud();
    if (!engine._inv_lives()) screen('over','Game Over','Score final '+engine._inv_score(),'Rejouer');
  }
  const visible = engine && ['playing','paused','over'].includes(state);
  const count = visible ? engine._inv_frame() : 0;
  const start = count ? engine._inv_frame_data() >>> 2 : 0;
  draw(mode,now/1000,count ? engine.HEAPF32.subarray(start,start+count*6) : null,count);
  requestAnimationFrame(tick);
}
requestAnimationFrame(tick);
try {
  engine = await globalThis.createInvaders();
  $('play').disabled = false;
  screen('menu','Prêt à décoller ?','Choisis ton mode graphique.','Jouer');
} catch (error) {
  console.error(error);
  $('status').textContent = 'Le moteur est indisponible. Recharge la page pour réessayer.';
}
