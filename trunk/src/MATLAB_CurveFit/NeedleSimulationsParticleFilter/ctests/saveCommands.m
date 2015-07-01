function saveCommands(us, pathBase, params)
uu = cell2mat(us);
dthetas = [uu.dtheta];
vs = [uu.v];

save(strcat(pathBase,'V.dat'), 'vs', '-ascii');
save(strcat(pathBase,'Thetas.dat'), 'dthetas', '-ascii');
end