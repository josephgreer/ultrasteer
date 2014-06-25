clear; clc; close all;

inputFile = 'D:\NEEDLE_GUIDANCE_HENRIETTA_123456\__20140611_180621_062000\DR_OVERVIEW_0023\NEEDLE_GUIDANCE_HENRIETTA.XA._.0023.0001.2014.06.16.14.49.06.781250.83596646.ima';

info = dicominfo(inputFile);

fig = figure('KeyPressFcn',@(obj,event)RespKeyPress(obj,event),'CloseRequestFcn',@(obj,event)RespCloseFunction(obj,event));

slashindices = strfind(inputFile,'\');
dotindices = strfind(inputFile,'.ima');
fname = inputFile(slashindices(length(slashindices))+1:dotindices(length(dotindices))-1);

handles = guidata(fig);
handles.index = 1;
handles.quit = 0;
handles.ptMap = containers.Map;
handles.inputFile = inputFile;
handles.info = info;
handles.fname = fname;
handles.scale = [1376 2907];
guidata(fig,handles);

if exist(strcat(fname,'.mat'),'file')
    load(strcat(fname,'.mat'));
end
guidata(fig,handles);

data = dicomread(inputFile,'frames',1);

imagesc(data(:,:),handles.scale);
colormap(gray);
axis equal;
