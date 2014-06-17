function varargout = rufCalibImageSelectionGUI(varargin)
% RUFCALIBIMAGESELECTIONGUI M-file for rufCalibImageSelectionGUI.fig
%      RUFCALIBIMAGESELECTIONGUI, by itself, creates a new RUFCALIBIMAGESELECTIONGUI or raises the existing
%      singleton*.
%
%      H = RUFCALIBIMAGESELECTIONGUI returns the handle to a new RUFCALIBIMAGESELECTIONGUI or the handle to
%      the existing singleton*.
%
%      RUFCALIBIMAGESELECTIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFCALIBIMAGESELECTIONGUI.M with the given input arguments.
%
%      RUFCALIBIMAGESELECTIONGUI('Property','Value',...) creates a new RUFCALIBIMAGESELECTIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufCalibImageSelectionGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufCalibImageSelectionGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufCalibImageSelectionGUI

% Last Modified by GUIDE v2.5 11-Jul-2006 15:30:18

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufCalibImageSelectionGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufCalibImageSelectionGUI_OutputFcn, ...
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



function rufCalibImagePreview(hObject, handles)
    global rufMainGUIData;
    currentImage = rufMainGUIData.CalibImage;
    axes(handles.Preview);
    colormap(gray);
    axis off;
    if rufMainGUIData.CalibImageIndex >= 0
        error = (1 == 0);
        % try to load
        try
            image = imread(strcat(handles.directory, currentImage));
        catch
            error = (1 == 1);
        end
        % if loaded, display
        if ~error
            imshow(image, 'DisplayRange', []);
            set(handles.ImageText, 'String', currentImage);
        else
            cla;
            set(handles.ImageText, 'String', strcat(currentImage, ' can not be loaded'));
        end
    else
        cla;
        set(handles.ImageText, 'String', 'None available');
    end

    
% --- Executes just before rufCalibImageSelectionGUI is made visible.
function rufCalibImageSelectionGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufCalibImageSelectionGUI (see VARARGIN)

    % Choose default command line output for rufCalibImageSelectionGUI
    handles.output = hObject;
    global rufMainGUIData;
    handles.directory = rufCalibRawDirectory(rufMainGUIData.CalibRootDirectory, rufMainGUIData.CalibSession);
    handles.allFiles = dir(strcat(handles.directory, '*.tiff'));
    rufCalibImagePreview(hObject, handles);
    
    % Update handles structure
    guidata(hObject, handles);

% UIWAIT makes rufCalibImageSelectionGUI wait for user response (see UIRESUME)
% uiwait(handles.rufCalibImageSelectionGUI);


% --- Outputs from this function are returned to the command line.
function varargout = rufCalibImageSelectionGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in PreviousButton.
function PreviousButton_Callback(hObject, eventdata, handles)
% hObject    handle to PreviousButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if length(handles.allFiles) ~= 0
        global rufMainGUIData;
        imageIndex = rufMainGUIData.CalibImageIndex;
        imageIndex = imageIndex - 1;
        if imageIndex <= 0
            imageIndex = length(handles.allFiles);
        end
        rufMainGUIData.CalibImageIndex = imageIndex;
        rufMainGUIData.CalibImage = handles.allFiles(imageIndex).name;
        rufCalibImagePreview(hObject, handles);
    end


% --- Executes on button press in NextButton.
function NextButton_Callback(hObject, eventdata, handles)
% hObject    handle to NextButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    if length(handles.allFiles) ~= 0
        global rufMainGUIData;
        imageIndex = rufMainGUIData.CalibImageIndex;
        imageIndex = imageIndex + 1;
        if imageIndex > length(handles.allFiles)
            imageIndex = 1;
        end
        rufMainGUIData.CalibImageIndex = imageIndex;
        rufMainGUIData.CalibImage = handles.allFiles(imageIndex).name;
        rufCalibImagePreview(hObject, handles);
    end


% --- Executes on button press in AcquireButton.
function AcquireButton_Callback(hObject, eventdata, handles)
% hObject    handle to AcquireButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    acquisitionHandle = rufAcquisitionGUI(handles.directory);
    uiwait(acquisitionHandle);
    handles.allFiles = dir(strcat(handles.directory, '*.tiff'));
    rufMainGUIData.CalibImageIndex = length(handles.allFiles);
    rufMainGUIData.CalibImage = handles.allFiles(rufMainGUIData.CalibImageIndex).name;
    rufCalibImagePreview(hObject, handles);
    guidata(hObject, handles);


% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufCalibImageSelectionGUI);

