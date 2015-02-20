%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% commandFcn
%%% inputs:
%%%     t = time 
%%%     params = simulationParameters initialized in initParams
%%% outputs:
%%%     uc = command to be executed this time step     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function uc = commandFcn(t, params)
global flipped;
if(t == 0)
    flipped = false;
end

uc.v = 10;          %10mm/s
uc.dc = 0;          %no-duty cycle

%flip it for an s-curve optionally
if(t > 100 && ~flipped)
    uc.dtheta = pi/2;
    flipped = 1;
else
    uc.dtheta = 0;
end
end