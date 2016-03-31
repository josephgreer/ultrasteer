%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% historyCommandFcn
%%% inputs:
%%%     t = time 
%%%     params = simulationParameters initialized in initParams
%%% outputs:
%%%     uc = command to be executed this time step     
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function uc = historyCommandFcn(t, params, us)
global index;
if(t == 0)
    index = 1;
end

uc = us{index};
index = index+1;

end