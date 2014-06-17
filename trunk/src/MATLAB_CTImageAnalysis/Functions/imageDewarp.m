function [] = imageDewarp(dewarpParams,imageFolder,dewarpFolder)

files = dir([imageFolder '/test*']); 
for i = 1:size(files)
    Filename = getfield(files(i),'name');
    I = imread([imageFolder Filename()],'tiff');
    dI = imtransform(I, dewarpParams,'xdata', [1, 480], 'ydata', [1, 480]);
 % imwrite(dI,[Folder 'DEWARPED/' Filename(1:length(Filename)-4) '_DW.jpg'], 'jpg');
 imwrite(dI,[dewarpFolder '/' Filename(1:length(Filename)-5) '.tiff'], 'tiff');
end 