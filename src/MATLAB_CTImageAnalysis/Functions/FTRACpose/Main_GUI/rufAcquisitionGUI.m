function varargout = rufAcquisitionGUI(varargin)
% RUFACQUISITIONGUI M-file for rufAcquisitionGUI.fig
%      RUFACQUISITIONGUI, by itself, creates a new RUFACQUISITIONGUI or raises the existing
%      singleton*.
%
%      H = RUFACQUISITIONGUI returns the handle to a new RUFACQUISITIONGUI or the handle to
%      the existing singleton*.
%
%      RUFACQUISITIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFACQUISITIONGUI.M with the given input arguments.
%
%      RUFACQUISITIONGUI('Property','Value',...) creates a new RUFACQUISITIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufAcquisitionGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufAcquisitionGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufAcquisitionGUI

% Last Modified by GUIDE v2.5 19-Jul-2007 17:31:19

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufAcquisitionGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufAcquisitionGUI_OutputFcn, ...
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


% --- Executes just before rufAcquisitionGUI is made visible.
function rufAcquisitionGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufAcquisitionGUI (see VARARGIN)

% Choose default command line output for rufAcquisitionGUI
    handles.output = hObject;
    handles.directory = varargin{1};
    handles.mode = 0; % 0 is for file, 1 for live video
    axes(handles.LastAquiredAxes);
    colormap(gray);
    cla;
    axis off;    

    % Update handles structure
    guidata(hObject, handles);

    

% --- Outputs from this function are returned to the command line.
function varargout = rufAcquisitionGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



% --- Executes on button press in AcquireButton.
function AcquireButton_Callback(hObject, eventdata, handles)
% hObject    handle to AcquireButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    set(handles.AcquireButton, 'Enable', 'off');
    allFiles = dir(strcat(handles.directory, '/img-*.*'));
    if length(allFiles) == 0
        number = 1;
    else
        lastFile = allFiles(length(allFiles)).name;      
        stringNumber = regexp(lastFile, '\d*', 'match');
        number = eval(cell2mat(stringNumber));
        number = number + 1;
    end
    fileName = sprintf('img-%04d.tiff', number);
    if handles.mode == 1
        start(handles.video);
        data = getdata(handles.video, 1);
        stop(handles.video);
        % for the gray scale images, the r,g,b components are all the
        % same, here we are using the first component as our image.
        image = data(:,:,1);
    else
        [filename, pathname] = uigetfile( ...
            {'*.*',  'All Files (*.*)'}, ...
            'Import an existing image', ...
            'MultiSelect', 'off');
        errorCode = (0 == 1);
        try
            image = imread(strcat(pathname, filename));
        catch
            errorCode = (1 == 1);
        end
        % if loaded, display
        if errorCode
            errordlg('Can not load file');
        end
    end
    % preview
    axes(handles.LastAquiredAxes);
    colormap(gray);
    imshow(image);
    axis off;    
    
    % and save
    imwrite(image, strcat(handles.directory, '/', fileName), 'tiff');
    set(handles.LastText, 'string', fileName);
    set(handles.AcquireButton, 'Enable', 'on');
    
    
    
% --- Executes on button press in CloseButton.
function CloseButton_Callback(hObject, eventdata, handles)
% hObject    handle to CloseButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if handles.mode == 1
        try
            closepreview(handles.video);
        catch
            errordlg('Can not close the video preview');
        end
    end
    delete(handles.rufAcquisitionGUI);


    
% --- Executes during object creation, after setting all properties.
function rufAcquisitionGUI_CreateFcn(hObject, eventdata, handles)
% hObject    handle to rufAcquisitionGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called




% --- Executes on button press in VideoCheckBox.
function VideoCheckBox_Callback(hObject, eventdata, handles)
% hObject    handle to VideoCheckBox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of VideoCheckBox
    newMode = get(hObject, 'Value');
    % create video object and add preview in GUI if needed
    if handles.mode ~= newMode
        handles.mode = newMode;
        if handles.mode == 1
            try
                handles.video = videoinput('winvideo', 1, 'RGB32_720x480');
                preview(handles.video);
            catch
                errordlg('Can not find a video input');
                handles.mode = 0;
                set(hObject, 'Value', 0);
            end
        else
            try
                closepreview(handles.video);
            catch
                errordlg('Can not close the video preview');
            end
        end
    end
    % Update handles structure
    guidata(hObject, handles);


% --- Executes on button press in ImportDicomButton.
function ImportDicomButton_Callback(hObject, eventdata, handles)
% hObject    handle to ImportDicomButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    [filenames, pathname] = uigetfile('*.*', ...
                                     'Select one or more Dicom file', ...
                                     'C:/Program Files/DICOMscope351/database/', ...
                                     'MultiSelect', 'on');

    % find number for next file
    allFiles = dir(strcat(handles.directory, '/img-*.*'));
    if length(allFiles) == 0
        number = 1;
    else
        lastFile = allFiles(length(allFiles)).name;      
        stringNumber = regexp(lastFile, '\d*', 'match');
        number = eval(cell2mat(stringNumber));
        number = number + 1;
    end

    foundOne = false;
    
    if ~iscellstr(filenames)
        filenames = cellstr(filenames);
    end
    
    for i = 1:length(filenames)
        filename = filenames{i};
        dicomFile = strcat(pathname, filename);
        info = dicominfo(dicomFile);
        x = dicomread(dicomFile);
        if ~isempty(x)
            fileNameTiff = sprintf('img-%04d.tiff', number);
            fileNameDcm = sprintf('img-%04d-', number);
            fileNameDcm = strcat(fileNameDcm, filename);
            % move original file
            movefile(dicomFile, strcat(handles.directory, '/', fileNameDcm));
            % and save
            imwrite(x, strcat(handles.directory, '/', fileNameTiff), 'tiff');
            number = number + 1;
            foundOne = true;
        else
            strcat(filename, ' does seem to be a dicom file')
        end
    end
    
    if foundOne
        set(handles.LastText, 'string', fileNameTiff);
        % preview
        axes(handles.LastAquiredAxes);
        colormap(gray);
        imshow(x, 'DisplayRange', []);
        axis off;    
    end
    
    % Update handles structure
    guidata(hObject, handles);


% --- Executes on button press in DatePrefixDicomButtom.
function DatePrefixDicomButtom_Callback(hObject, eventdata, handles)
% hObject    handle to DatePrefixDicomButtom (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    [filenames, pathname] = uigetfile('*.*', ...
                                     'Select one or more Dicom file', ...
                                     'C:/Program Files/DICOMscope351/database/', ...
                                     'MultiSelect', 'on');    
    if ~iscellstr(filenames)
        filenames = cellstr(filenames);
    end
    
    for i = 1:length(filenames)
        filename = filenames{i};
        dicomFile = strcat(pathname, filename);
        info = dicominfo(dicomFile);
        x = dicomread(dicomFile);
        if ~isempty(x)
            newFileName = strcat(pathname, info.ContentDate, '-', info.ContentTime, '-', filename);
            movefile(dicomFile, newFileName);
        else
            strcat(filename, ' does seem to be a dicom file')
        end
    end
    
    % Update handles structure
    guidata(hObject, handles);


