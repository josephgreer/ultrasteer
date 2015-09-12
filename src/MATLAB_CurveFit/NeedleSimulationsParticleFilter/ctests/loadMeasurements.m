function measurements = saveMeasurements(pathBase)
doppler = load(strcat(pathBase, 'MeasDoppler.dat'));
pos = load(strcat(pathBase,'MeasPos.dat'));
fuls = load(strcat(pathBase,'MeasFul.dat'));
fbls = load(strcat(pathBase,'MeasFbl.dat'));
fbrs = load(strcat(pathBase,'MeasFbr.dat'));
furs = load(strcat(pathBase,'MeasFur.dat'));
fbxs = load(strcat(pathBase,'MeasBx.dat'));
fbys = load(strcat(pathBase,'MeasBy.dat'));
uvs = load(strcat(pathBase,'MeasUv.dat'));


measurements = {};
for i=1:length(doppler)
    mm.doppler = doppler(i);
    mm.pos = pos(:,i);
    mm.ful = fuls(:,i);
    mm.fbl = fbls(:,i);
    mm.fbr = fbrs(:,i);
    mm.fur = furs(:,i);
    mm.bx = fbxs(:,i);
    mm.by = fbys(:,i);
    mm.uv = uvs(i);
    
    measurements = vertcat(measurements, mm);
end
end