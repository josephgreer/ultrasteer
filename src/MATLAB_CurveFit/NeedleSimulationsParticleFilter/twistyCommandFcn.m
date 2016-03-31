%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% commandFcn
%%% inputs:
%%%     t = time 
%%%     params = simulationParameters initialized in initParams
%%% outputs:
%%%     uc = command to be executed this time step     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function uc = commandFcn(t, params)

uc.v = 10;          %10mm/s
uc.dc = 0;          %no-duty cycle

%flip it for an s-curve optionally
if(t == 1)
    uc.dtheta = pi/4;
elseif(t == 6)
    uc.dtheta = pi/6;    
elseif(t == 8)
    uc.dtheta = pi/3;
elseif(t == 10)
    uc.dtheta = pi;
elseif(t == 12)
    uc.dtheta = pi;
else 
    uc.dtheta = 0;
end
end