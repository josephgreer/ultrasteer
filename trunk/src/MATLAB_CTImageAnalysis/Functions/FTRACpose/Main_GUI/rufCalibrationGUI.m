function varargout = rufCalibrationGUI(varargin)
% RUFCALIBRATIONGUI M-file for rufCalibrationGUI.fig
%      RUFCALIBRATIONGUI, by itself, creates a new RUFCALIBRATIONGUI or raises the existing
%      singleton*.
%
%      H = RUFCALIBRATIONGUI returns the handle to a new RUFCALIBRATIONGUI or the handle to
%      the existing singleton*.
%
%      RUFCALIBRATIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFCALIBRATIONGUI.M with the given input arguments.
%
%      RUFCALIBRATIONGUI('Property','Value',...) creates a new RUFCALIBRATIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufCalibrationGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufCalibrationGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Copyright 2002-2003 The MathWorks, Inc.

% Edit the above text to modify the response to help rufCalibrationGUI

% Last Modified by GUIDE v2.5 31-Jul-2007 16:47:49

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufCalibrationGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufCalibrationGUI_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before rufCalibrationGUI is made visible.
function rufCalibrationGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufCalibrationGUI (see VARARGIN)
    % Choose default command line output for rufCalibrationGUI
    handles.output = hObject;

    global rufMainGUIData;
    handles.rawDirectory = rufCalibRawDirectory(rufMainGUIData.CalibRootDirectory, rufMainGUIData.CalibSession);
    handles.processedDirectory = rufCalibrationProcessedDirectory(rufMainGUIData.CalibRootDirectory, rufMainGUIData.CalibSession);
    if ~isdir(handles.processedDirectory)
        mkdir(handles.processedDirectory);
    end
    handles.originalImage = rufMainGUIData.CalibImage;
    [pathstr, handles.session, ext, versn] = fileparts(handles.originalImage);
    
    
    
    % display original image
    axes(handles.OriginalAxes);
    colormap(gray);
    axis off;
    handles.image = imread(strcat(handles.rawDirectory, handles.originalImage));
    imshow(handles.image, 'DisplayRange', []);
    
    % look at the size to deduct the resolution.  This is a last minute
    % hack.  By default, we have been using NTSC videos, ie. 480 x 480
    % usable field of view.  The resolution ratio is compared to NTSC.
    xySize = size(handles.image);
    handles.resRatioToNTSC = xySize(1) / 480;
    
    % clear others
    axes(handles.MaskAxes);
    colormap(gray);
    axis off;    
    cla;
    axes(handles.MaskedAxes);
    colormap(gray);
    axis off;    
    cla;

    axes(handles.BBAxes);
    colormap(gray);
    axis off;    
    cla;
    axes(handles.LineAxes);
    colormap(gray);
    axis off;    
    cla;
    
    % Update handles structure
    guidata(hObject, handles);


% UIWAIT makes rufCalibrationGUI wait for user response (see UIRESUME)
% uiwait(handles.rufCalibrationGUI);


% --- Outputs from this function are returned to the command line.
function varargout = rufCalibrationGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % Get default command line output from handles structure
    varargout{1} = handles.output;

    
    

% --- Function used to compute the mask and apply it to incoming images.    
function ComputeMask(hObject, handles, maskMode)
% hObject     handle to figure provided by callback
% handles     structure provided by callback
% maskMode    1 will look for lartgest dark area after smoothing
%               the image (good for NTSC video).
%             2 cercle centered on image, good for square DICOM images
%             3 no mask
%
    global rufMainGUIData;
    
    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    % clear others
    axes(handles.MaskAxes);
    colormap(gray);
    axis off;    
    cla;
    axes(handles.MaskedAxes);
    colormap(gray);
    axis off;    
    cla;

    axes(handles.BBAxes);
    colormap(gray);
    axis off;    
    cla;
    axes(handles.LineAxes);
    colormap(gray);
    axis off;    
    cla;
    
    drawnow;

    % normalize input image to double
    doubleImage = double(handles.image);
    maxValue = max(max(doubleImage));
    doubleImage = doubleImage ./ maxValue;
    switch maskMode
        case 1
            handles.mask = Generate_Image_Mask(handles.image);
        case 2
            handles.mask = ones(size(doubleImage));
            lX = size(doubleImage, 1);
            lY = size(doubleImage, 2)
            centerX =  lX / 2;
            centerY =  lY / 2;
            radius = min(centerX, centerY);
            for i = 1:lX
                for j = 1:lY
                    norm = sqrt((centerX - i) * (centerX - i) + (centerY - j) * (centerY - j));
                    if (norm > radius)
                        handles.mask(i, j) = 0;
                    end
                end
            end
        case 3
            handles.mask = ones(size(doubleImage));
        otherwise
            disp('Unknown method.')
    end
    
    % apply to input image
    imageMaskedDouble = doubleImage .* handles.mask;
    handles.imageMasked = uint8(imageMaskedDouble .* 256);
    
    % step 1: do the morphological processing
    handles.morphIm = PerformMorphologicalOperation(handles.imageMasked, handles.resRatioToNTSC);
    % step 2: compute automatic threshold
    [N,X] = hist(double(handles.morphIm(:)), 5);
    handles.dewarpThreshold = round(X(2));
    set(handles.DewarpThresholdInput, 'String', handles.dewarpThreshold);
    set(handles.DewarpThresholdSlider, 'Value', handles.dewarpThreshold);
    
    % display results
    axes(handles.MaskAxes);
    axis off;
    imshow(handles.mask);
    axes(handles.MaskedAxes);
    axis off;
    imshow(handles.imageMasked, 'DisplayRange', []);

    % save results
    imwrite(handles.mask, strcat(handles.processedDirectory, handles.session, '-mask.tiff'));
        
    % show we done
    set(gcf,'Pointer','arrow');
    drawnow;
    
    % sync data
    guidata(hObject, handles);        
    
    


% --- Executes on button press in MaskButton.
function MaskButton_Callback(hObject, eventdata, handles)
% hObject    handle to MaskButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    ComputeMask(hObject, handles, 1);
    


% --- Executes on button press in DicomMaskButton.
function DicomMaskButton_Callback(hObject, eventdata, handles)
% hObject    handle to DicomMaskButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    ComputeMask(hObject, handles, 2);



% --- Executes on button press in NoMaskButton.
function NoMaskButton_Callback(hObject, eventdata, handles)
% hObject    handle to NoMaskButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    ComputeMask(hObject, handles, 3);

    

% --- Executes on button press in DewarpButton.
function DewarpButton_Callback(hObject, eventdata, handles)
% hObject    handle to DewarpButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    
    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    try
        % clear others
        axes(handles.BBAxes);
        colormap(gray);
        axis off;    
        cla;
        axes(handles.LineAxes);
        colormap(gray);
        axis off;    
        cla;
        drawnow;
    
        handles.pixelResolution = [0.44 , 0.44];  % was 0.44, 0.44 for NTSC videos
        handles.pixelResolution = handles.pixelResolution ./ handles.resRatioToNTSC;
        [handles.dewarpImage, handles.Xmap, handles.Ymap, handles.PolyOrder, handles.PolyCoeff, handles.orientation, handles.origin, handles.gridPoints, handles.pixelResolution] = ComputeDistortionMap(handles.imageMasked, handles.morphIm, handles.dewarpThreshold, handles.resRatioToNTSC, handles.pixelResolution);
        % [handles.dewarpImage, handles.Xmap, handles.Ymap, handles.PolyOrder, handles.PolyCoeff, handles.orientation, handles.origin, handles.gridPoints, handles.pixelResolution] = ComputeDistortionMap(handles.image, handles.morphIm, handles.dewarpThreshold);
    
        % display results
        axes(handles.BBAxes);
        axis off;    
        imshow(handles.dewarpImage, 'DisplayRange', []);
        hold on;
        plot(handles.gridPoints(:,2), handles.gridPoints(:,1), 'ro');
        hold off;

        results{1} = 'Results:';
        results{2} = strcat('  Pixel resolution: ', num2str(handles.pixelResolution(1)), ', ', num2str(handles.pixelResolution(2)));
        set(handles.ResultsLabel, 'String', results);
    
        % save results
        imwrite(handles.dewarpImage, strcat(handles.processedDirectory, handles.session, '-dewarped.tiff'));
        WriteMappedCoordsToFile(strcat(handles.processedDirectory, handles.session, '-map-coords.txt'), handles.Xmap, handles.Ymap);
        WritePolyCoeffToFile(strcat(handles.processedDirectory, handles.session, '-poly-coeff.txt'), handles.PolyOrder, handles.PolyCoeff);
        [N,X] = hist(double(handles.dewarpImage(:)), 5);
        handles.calibrationThreshold = round(X(4) * 100); % int between 0 and 100
        set(handles.CalibrationThresholdSlider, 'Value', handles.calibrationThreshold);
        set(handles.CalibrationThresholdInput, 'String', handles.calibrationThreshold);

    catch
        err = lasterror;
        disp('--- Dewarping Error message ---');
        disp(err.message);
        set(gcf,'Pointer','arrow');
        drawnow;
        errordlg('Can not dewarp, try different mask, threshold or image','Dewarping Error')
    end
    
    % show we are done
    set(gcf,'Pointer','arrow');
    drawnow;
    
    % sync data
    guidata(hObject, handles);


% --- Executes on button press in CalibrateButton.
function CalibrateButton_Callback(hObject, eventdata, handles)
% hObject    handle to CalibrateButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    try
        % clear others
        axes(handles.LineAxes);
        colormap(gray);
        axis off;    
        cla;
        drawnow;

        doubleThreshold = handles.calibrationThreshold / 100.0; % double between 0 and 1 
        [handles.focalLength, handles.sourcePosition, handles.lineEndPoints] = ComputeCalibrationParameters(handles.dewarpImage, handles.pixelResolution, handles.orientation, handles.origin, handles.gridPoints, doubleThreshold, handles.resRatioToNTSC);
        axes(handles.LineAxes);
        axis off;    
        imshow(handles.dewarpImage, 'DisplayRange', []);
        numLines = size(handles.lineEndPoints, 1);
        for i = 1:numLines
            p = [handles.lineEndPoints(i,1:2); handles.lineEndPoints(i,3:4)];
            hold on;
            plot(p(:,2), p(:,1), '--g');
            hold off;
        end
        filename = strcat(handles.processedDirectory, handles.session, '-focal-length.txt');
        fileId = fopen(filename, 'wt');
        fprintf(fileId, '%8.8f\n', handles.focalLength);
        fclose(fileId);
        filename = strcat(handles.processedDirectory, handles.session, '-pixel-size.txt');
        fileId = fopen(filename, 'wt');
        fprintf(fileId, '%8.8f\n', handles.pixelResolution);
        fclose(fileId);
        filename = strcat(handles.processedDirectory, handles.session, '-origin.txt');
        fileId = fopen(filename, 'wt');
        fprintf(fileId, '%8.8f\n', handles.sourcePosition(1:2));
        fclose(fileId);
    
        results{1} = 'Results:';
        results{2} = strcat('  Pixel resolution: ', num2str(handles.pixelResolution(1)), ', ', num2str(handles.pixelResolution(2)));
        results{3} = strcat('  Focal distance: ', num2str(handles.focalLength)); 
        results{4} = strcat('  Origin: ', num2str(handles.sourcePosition(1)), ', ', num2str(handles.sourcePosition(2)));
        set(handles.ResultsLabel, 'String', results);
    catch
        err = lasterror;
        disp('--- Calibration Error message ---');
        disp(err.message);
        set(gcf,'Pointer','arrow');
        drawnow;
        errordlg('Can not calibrate, try different mask, threshold or image','Calibration Error')
    end
    
    % show we done
    set(gcf,'Pointer','arrow');
    drawnow;

    guidata(hObject, handles);

% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufCalibrationGUI);




% --- Executes on slider movement.
function DewarpThresholdSlider_Callback(hObject, eventdata, handles)
% hObject    handle to DewarpThresholdSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
    % sync data
    handles.dewarpThreshold = round(get(hObject,'Value'));
    set(handles.DewarpThresholdSlider, 'Value', handles.dewarpThreshold);
    set(handles.DewarpThresholdInput, 'String', handles.dewarpThreshold);
    guidata(hObject, handles);

    
    
% --- Executes during object creation, after setting all properties.
function DewarpThresholdSlider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to DewarpThresholdSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background, change
%       'usewhitebg' to 0 to use default.  See ISPC and COMPUTER.
usewhitebg = 1;
if usewhitebg
    set(hObject,'BackgroundColor',[.9 .9 .9]);
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function DewarpThresholdInput_Callback(hObject, eventdata, handles)
% hObject    handle to DewarpThresholdInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of DewarpThresholdInput as text
%        str2double(get(hObject,'String')) returns contents of DewarpThresholdInput as a double
    handles.dewarpThreshold = round(str2double(get(hObject,'String')));
    if handles.dewarpThreshold < 0
        handles.dewarpThreshold = 0;
    end
    if handles.dewarpThreshold > 100
        handles.dewarpThreshold = 100;
    end
    set(handles.DewarpThresholdSlider, 'Value', handles.dewarpThreshold);
    set(handles.DewarpThresholdInput, 'String', handles.dewarpThreshold);
    % sync data
    guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function DewarpThresholdInput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to DewarpThresholdInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end




% --- Executes on slider movement.
function CalibrationThresholdSlider_Callback(hObject, eventdata, handles)
% hObject    handle to CalibrationThresholdSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
    handles.calibrationThreshold = round(get(hObject,'Value'));
    set(handles.CalibrationThresholdSlider, 'Value', handles.calibrationThreshold);
    set(handles.CalibrationThresholdInput, 'String', handles.calibrationThreshold);
    guidata(hObject, handles);


% --- Executes during object creation, after setting all properties.
function CalibrationThresholdSlider_CreateFcn(hObject, eventdata, handles)
% hObject    handle to CalibrationThresholdSlider (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function CalibrationThresholdInput_Callback(hObject, eventdata, handles)
% hObject    handle to CalibrationThresholdInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of CalibrationThresholdInput as text
%        str2double(get(hObject,'String')) returns contents of CalibrationThresholdInput as a double
    handles.calibrationThreshold = round(str2double(get(hObject,'String')));
    if handles.calibrationThreshold < 0
        handles.calibrationThreshold = 0;
    end
    if handles.calibrationThreshold > 100
        handles.calibrationThreshold = 100;
    end
    set(handles.CalibrationThresholdSlider, 'Value', handles.calibrationThreshold);
    set(handles.CalibrationThresholdInput, 'String', handles.calibrationThreshold);
    % sync data
    guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function CalibrationThresholdInput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to CalibrationThresholdInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

