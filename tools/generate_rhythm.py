"""Create three original PCM tracks synchronized to the game's beat maps (stdlib only)."""
from pathlib import Path
import math,wave,array
out=Path('assets/rhythm');out.mkdir(parents=True,exist_ok=True)
rate=22050;names=['neon','ion','nova'];bpms=[96,120,144]
for track,(name,bpm) in enumerate(zip(names,bpms)):
    period=60/bpm;duration=2+64*period+2;frames=int(rate*duration);samples=array.array('h')
    scale=[261.6256,329.6276,391.9954,523.2511]
    for i in range(frames):
        t=i/rate;beat=(t-2)/period;value=0.0
        if 0<=beat<64:
            n=int(beat);phase=(beat-n)*period;lane=(n*3+n//8+track)%4;freq=scale[lane]*2**(track/12)
            value+=.19*math.exp(-phase*8)*(math.sin(2*math.pi*freq*phase)+.22*math.sin(4*math.pi*freq*phase))
            value+=.22*math.exp(-phase*24)*math.sin(2*math.pi*(55*phase+22*(1-math.exp(-phase*30))/30))
            hat=(beat*2)%1*period/2
            value+=.045*math.exp(-hat*75)*math.sin(2*math.pi*7900*t)*math.sin(2*math.pi*1237*t)
            if track and n%4==2:value+=.07*math.exp(-phase*16)*math.sin(2*math.pi*1700*t)
        samples.append(int(max(-.9,min(.9,value))*32767))
    with wave.open(str(out/(name+'.wav')),'wb') as f:f.setnchannels(1);f.setsampwidth(2);f.setframerate(rate);f.writeframes(samples.tobytes())
    print(name,bpm,round(duration,2),'seconds',flush=True)
