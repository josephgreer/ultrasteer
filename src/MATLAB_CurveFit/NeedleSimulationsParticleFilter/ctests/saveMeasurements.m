function saveMeasurements(measurements, pathBase, params)
mm = cell2mat(measurements);
doppler = [mm.doppler];
pos = [mm.pos];
ful = [mm.ful];
fbl = [mm.fbl];
fbr = [mm.fbr];
fur = [mm.fur];
bx = [mm.bx];
by = [mm.by];
uv = [mm.uv];


save(strcat(pathBase,'MeasDoppler.dat'), 'doppler', '-ascii');
save(strcat(pathBase,'MeasPos.dat'), 'pos', '-ascii');
save(strcat(pathBase,'MeasFul.dat'), 'ful', '-ascii');
save(strcat(pathBase,'MeasFbl.dat'), 'fbl', '-ascii');
save(strcat(pathBase,'MeasFbr.dat'), 'fbr', '-ascii');
save(strcat(pathBase,'MeasFur.dat'), 'fur', '-ascii');
save(strcat(pathBase,'MeasBx.dat'), 'bx', '-ascii');
save(strcat(pathBase,'MeasBy.dat'), 'by', '-ascii');
save(strcat(pathBase,'MeasUv.dat'), 'uv', '-ascii');
end