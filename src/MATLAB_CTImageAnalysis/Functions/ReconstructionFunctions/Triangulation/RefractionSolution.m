%Solves the geometry of the exit refraction to determine the point at which
%the exit refraction ends

% ng = refractive index of the gel
% see diagram for the other variable definitions

%clear all
%syms Ak Wk An Wn refIndex ng
%RefractionSolution = solve (refIndex^2 * (Wk -Ak)^2*(Ak^2 + An^2) - Ak^2 * ((Wn- An)^2+(Wk - Ak)^2),Ak);
%RefractionSolution = solve (refIndex^2 * (Wk_s -Ak_s)^2*(Ak_s^2 + An_s^2) - Ak_s^2 * ((Wn_s- An_s)^2+(Wk_s - Ak_s)^2),Ak_s);
%RefractionSolution = simplify(RefractionSolution);

%clear Ak Wk An Wn refIndex ng

%specificSolution = RefractionSolution(3);

%save refractionSolution


%% Try solving with solve (%slower solution but will solve)
% syms Ak;
% Solution = solve (refIndex^2 * (Wk -Ak)^2*(Ak^2 + An^2) - Ak^2 * ((Wn- An)^2+(Wk - Ak)^2),Ak);
% Ak = double(Solution (3))


%% Try solving with fzero (faster solution, but can fail)
guess1 = An/ tan (atan2(Wn, Wk));
guess2 = Wk;
guess = (guess1 + guess2)/2;

%Ak = fzero(@(Ak) refractionOut(Ak, Wk, An, Wn, refIndex),guess);
Ak = fzero(@(Ak) refractionOut(Ak, Wk, An, Wn, refIndex),[guess1 guess2]);

% If it didn't solve, try the bounaries. 
if max(max(isnan(Ak)))
    guess = guess2;
    Ak = fzero(@(Ak) refractionOut(Ak, Wk, An, Wn, refIndex),guess);
    if max(max(isnan(Ak)))
        guess = guess1;
        Ak = fzero(@(Ak) refractionOut(Ak, Wk, An, Wn, refIndex),guess);

    end
end
