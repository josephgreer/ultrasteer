function SetCam(above, ul, bl, ur, p)
if(~above)
cameraStandbackVector = ul-ur;%(ul-bl);
cameraStandbackVector = transpose(cameraStandbackVector/norm(cameraStandbackVector));
cameraUpVector = ul-bl;%cross(bl-ul, ur-ul);
cameraUpVector = transpose(cameraUpVector/norm(cameraUpVector));
cameraLookVector = p;
cameraPos = cameraLookVector+cameraStandbackVector*100;
else
cameraStandbackVector = (ul-bl);
cameraStandbackVector = transpose(cameraStandbackVector/norm(cameraStandbackVector));
cameraUpVector = cross(bl-ul, ur-ul);
cameraUpVector = transpose(cameraUpVector/norm(cameraUpVector));
cameraLookVector = p;
cameraPos = cameraLookVector+cameraStandbackVector*100;
end

campos('manual');
campos(cameraPos);
camtarget(cameraLookVector);
camup(cameraUpVector);
axis equal;
end

