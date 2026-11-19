const assert = require('node:assert/strict');
const createInvaders = require('./engine.js');

async function test() {
  const engine = await createInvaders();
  engine._inv_reset(2);
  assert.equal(engine._inv_wave(), 1);
  assert.equal(engine._inv_lives(), 3);
  assert.equal(engine._inv_frame(), 17);
  const frame = () => engine.HEAPF32[engine._inv_frame_data() >>> 2];
  assert.equal(frame(), 304);
  for (let i = 0; i < 40; i++) engine._inv_step(1,0,1);
  assert.ok(engine._inv_frame() > 17);
  assert.ok(frame() > 304);
  assert.equal(engine._inv_score(), 0);
}
test().catch(error => {
  console.error(error);
  process.exitCode = 1;
});
