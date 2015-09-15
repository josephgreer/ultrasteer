function us = loadCommands(pathBase)
vs = load(sprintf('%sV.dat',pathBase));
dthetas = load(sprintf('%sThetas.dat',pathBase));
dutyCycles = load(sprintf('%sDutyCycles.dat', pathBase));

us = {};
for i=1:length(vs)
    u.v = vs(i);
    u.dutyCycle = dutyCycles(i);
    u.dtheta = dthetas(i);
    us = vertcat(us,u);
end
end