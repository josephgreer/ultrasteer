clear; clc; close all;

%filename
baseDir = 'C:\Users\Joey\Dropbox (Stanford CHARM Lab)\ICRA 2015 EM Needle Vibration\Experimental Data (Ultrasound)\8_26_14 Experiment (Artificial Tissue)\8 cm Depth\';
files = dir(strcat(baseDir,'15-03-47.col'));


for fi = 1:length(files)
    fname = files(fi).name;
    file = fopen(strcat(baseDir,fname), 'rb');
    header = ReadHeader(file);
    responses = [];
    for fr = 1:header.nframes
        im = RPreadframe(file, header);
        im = im([header.ul(2):header.br(2)],[header.ul(1):header.br(1)],:);
        imR = im(:,:,1); imG = im(:,:,2); imB = im(:,:,3);
        trueMat = (abs(im(:,:,1)-im(:,:,2))<4 & abs(im(:,:,2)-im(:,:,3))<4 & abs(im(:,:,1)-im(:,:,3)<4));
        indices = find(trueMat > 0);
        imR(indices) = 0;
        imG(indices) = 0;
        imB(indices) = 0;
        im(:,:,1) = imR;  im(:,:,2) = imG; im(:,:,3) = imB;
        responses(fr) = sum(sum(imR+imG+imB))/3;
        imshow(im);
    end
    saveName = strcat(baseDir,fname(1:strfind(fname,'.col')),'mat');
    save(saveName,'responses');
    fclose(file);
end