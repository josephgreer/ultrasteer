mainDir = '~/Desktop/temp/Actuator_3';
desiredOutIndex = 1320;

list = rdir(mainDir);
list = list(2:end);
nms = [];
for i=1:length(list)
    nms(i) = str2double(list(i).name(20:23));
end

inIndex = find(nms == desiredOutIndex);
outIndex = (inIndex-1)*8-1