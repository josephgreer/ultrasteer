% If val = 0, then Ak == true Ak

function val = refractionOut (Ak, Wk, An, Wn, refIndex)

val = refIndex^2 * (Wk -Ak)^2*(Ak^2 + An^2) - Ak^2 * ((Wn- An)^2+(Wk - Ak)^2);