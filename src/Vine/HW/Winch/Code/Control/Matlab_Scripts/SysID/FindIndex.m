mainDir = '~/Desktop/temp/Actuator_3';
desiredIndex = 215;

list = rdir(mainDir);
list = list(2:end);
nms = [];
for i=1:length(list)
    nms(i) = str2double(list(i).name(20:23));
end

inIndex = ((desiredIndex+1)/8)+1
outIndex = nms(inIndex)