const W = 640, H = 720;
const ink = '#0c1225', mint = '#69fac4', pink = '#ff72a3';

export function renderer(canvas) {
  const ctx = canvas.getContext('2d', {alpha: false});
  const box = (x,y,w,h,color) => {
    ctx.fillStyle = color;
    ctx.fillRect(x,y,w,h);
  };
  const classic = document.createElement('canvas');
  const enhanced = document.createElement('canvas');
  for (const bg of [classic,enhanced]) {
    bg.width = W;
    bg.height = H;
  }
  const flat = classic.getContext('2d');
  flat.fillStyle = ink;
  flat.fillRect(0,0,W,H);
  flat.fillStyle = '#384a65';
  for (let i=0; i<64; i++) flat.fillRect((i*173+41)%W,(i*i*7+79*i)%H,2,2);

  const sky = enhanced.getContext('2d');
  sky.fillStyle = '#070c1c';
  sky.fillRect(0,0,W,H);
  for (const [x,y,r,color] of [[468,152,320,'#493186'],[144,388,265,'#17526d'],[325,660,215,'#154458']]) {
    const glow = sky.createRadialGradient(x,y,0,x,y,r);
    glow.addColorStop(0,color);
    glow.addColorStop(1,'transparent');
    sky.fillStyle = glow;
    sky.fillRect(0,0,W,H);
  }
  sky.strokeStyle = '#93b7ff20';
  for (let x=0; x<W; x+=40) {
    sky.beginPath(); sky.moveTo(x,0); sky.lineTo(x,H); sky.stroke();
  }
  for (let y=0; y<H; y+=40) {
    sky.beginPath(); sky.moveTo(0,y); sky.lineTo(W,y); sky.stroke();
  }
  const stars = Array.from({length:120},(_,i) => ({
    x: (i*173+41)%W, y: (i*i*7+79*i)%H, radius: i%13===0 ? 2 : 1, speed: 9+i%20
  }));

  function classicSprite(x,y,w,h,kind,shield) {
    if (kind === 0) {
      const color = shield ? '#e1eaf8' : mint;
      box(x,y+12,w,8,color);
      box(x+8,y+7,16,8,color);
      box(x+13,y,6,8,color);
    } else if (kind === 1) {
      box(x+5,y,20,5,pink);
      box(x,y+5,30,12,pink);
      box(x+4,y+17,5,5,pink);
      box(x+21,y+17,5,5,pink);
      box(x+7,y+8,4,4,ink);
      box(x+19,y+8,4,4,ink);
    } else {
      box(x,y,w,h,kind === 2 ? mint : pink);
    }
  }
  function enhancedSprite(x,y,w,h,kind,shield,time) {
    const color = kind === 0 || kind === 2 ? '#79ffdd' : '#ff6ab2';
    ctx.save();
    ctx.translate(x,y);
    ctx.shadowColor = color;
    ctx.shadowBlur = kind < 2 ? 12 : 15;
    if (kind >= 2) {
      box(0,kind===2 ? 3 : -8,w,h+8,kind===2 ? '#4ffaaf64' : '#ff6ab26c');
      box(0,0,w,h,color);
      box(1,1,w-2,h-2,'#fff5ff');
    } else if (kind === 0) {
      ctx.fillStyle = '#60b9ff';
      ctx.beginPath();
      ctx.moveTo(11,17); ctx.lineTo(16,25+Math.sin(time*35)*3); ctx.lineTo(21,17);
      ctx.fill();
      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.moveTo(16,0); ctx.lineTo(w,17); ctx.lineTo(24,h); ctx.lineTo(16,16); ctx.lineTo(8,h); ctx.lineTo(0,17);
      ctx.closePath(); ctx.fill();
      box(14,8,4,7,'#142d57');
      if (shield) {
        ctx.strokeStyle = '#aaf3ff';
        ctx.beginPath(); ctx.ellipse(w/2,h/2,w*.7,h*.8,0,0,Math.PI*2); ctx.stroke();
      }
    } else {
      ctx.fillStyle = color;
      ctx.beginPath();
      ctx.moveTo(0,8); ctx.lineTo(7,0); ctx.lineTo(w-7,0); ctx.lineTo(w,8);
      ctx.lineTo(w-4,h); ctx.lineTo(w/2,16); ctx.lineTo(4,h);
      ctx.closePath(); ctx.fill();
      box(6,7,18,5,'#321746');
      box(8,8,4,3,'#ffe3fa'); box(18,8,4,3,'#ffe3fa');
    }
    ctx.restore();
  }
  return (mode, time, frame, count) => {
    ctx.drawImage(mode === 'classic' ? classic : enhanced,0,0);
    if (mode === 'enhanced') {
      for (const star of stars)
        box(star.x,(star.y+time*star.speed)%H,star.radius,star.radius,star.radius===2 ? '#e0e9ff' : '#a9b9fb77');
    }
    for (let i=0; i<count*6; i+=6) {
      const x=frame[i], y=frame[i+1], w=frame[i+2], h=frame[i+3], kind=frame[i+4], shield=frame[i+5];
      if (mode === 'classic') classicSprite(x,y,w,h,kind,shield);
      else enhancedSprite(x,y,w,h,kind,shield,time);
    }
  };
}
