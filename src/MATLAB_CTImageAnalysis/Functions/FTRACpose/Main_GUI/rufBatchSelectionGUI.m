function varargout = rufBatchSelectionGUI(varargin)
% RUFBATCHSELECTIONGUI M-file for rufBatchSelectionGUI.fig
%      RUFBATCHSELECTIONGUI, by itself, creates a new RUFBATCHSELECTIONGUI or raises the existing
%      singleton*.
%
%      H = RUFBATCHSELECTIONGUI returns the handle to a new RUFBATCHSELECTIONGUI or the handle to
%      the existing singleton*.
%
%      RUFBATCHSELECTIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFBATCHSELECTIONGUI.M with the given input arguments.
%
%      RUFBATCHSELECTIONGUI('Property','Value',...) creates a new RUFBATCHSELECTIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufBatchSelectionGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufBatchSelectionGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufBatchSelectionGUI

% Last Modified by GUIDE v2.5 04-Apr-2006 10:20:18

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufBatchSelectionGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufBatchSelectionGUI_OutputFcn, ...
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


% Refresh thumbnails
function rufBatchSelectionThumbnails(handles)
    global rufMainGUIData;
    set(handles.BatchText, 'String', rufMainGUIData.Batch);
    rawDirectory = rufPatientRawDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.PatientID);
    directory = strcat(rawDirectory, '/', rufMainGUIData.Batch, '/');
    allFiles = dir(strcat(directory, '*.*'));
    currentPreview = 1;
    currentFile = 3; % for directories . and ..
    while (currentPreview <= 12)
        
        % set current view
        currentAxes = eval(sprintf('handles.Preview%d', currentPreview));
        axes(currentAxes);

        % see if there are some files left
        if (currentFile <= length(allFiles))
            error = (1 == 0);
            % try to load
            try
                imageValue = imread(strcat(directory, allFiles(currentFile).name));
            catch
                error = (1 == 1);
            end
            % if loaded, display
            if ~error
                % select and set current axes
                colormap(gray);
                imshow(imageValue, 'DisplayRange', []);
                axis off;
                currentPreview = currentPreview + 1;
            end
            % get ready for next image
            currentFile = currentFile + 1;
        else
            % no file left, clear axes
            colormap(gray);
            cla;
            axis off;
            currentPreview = currentPreview + 1;
        end
    end
    

% --- Executes just before rufBatchSelectionGUI is made visible.
function rufBatchSelectionGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufBatchSelectionGUI (see VARARGIN)
    global rufMainGUIData;
    directory = rufPatientRawDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.PatientID);
    set(handles.CurrentDirectoryText, 'String', directory);
    set(handles.BatchText, 'String', rufMainGUIData.Batch);
    rufBatchSelectionThumbnails(handles);
    

% Choose default command line output for rufBatchSelectionGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes rufBatchSelectionGUI wait for user response (see UIRESUME)
% uiwait(handles.rufBatchSelectionGUI);


% --- Outputs from this function are returned to the command line.
function varargout = rufBatchSelectionGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in PreviousBatchButton.
function PreviousBatchButton_Callback(hObject, eventdata, handles)
% hObject    handle to PreviousBatchButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    directory = rufPatientRawDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.PatientID);
    allFiles = dir(strcat(directory, '/batch-*'));
    found = 0;
    current = 1;
    while ((current <= length(allFiles)) && (found == 0))
        if strcmp(allFiles(current).name, rufMainGUIData.Batch)
            found = current;
        end
        current = current + 1;
    end
    found = found - 1;
    if found == 0
        found = length(allFiles);
    end
    rufMainGUIData.Batch = allFiles(found).name;
    rufBatchSelectionThumbnails(handles);


% --- Executes on button press in NextBatchButton.
function NextBatchButton_Callback(hObject, eventdata, handles)
% hObject    handle to NextBatchButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    directory = rufPatientRawDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.PatientID);
    allFiles = dir(strcat(directory, '/batch-*'));
    found = 0;
    current = 1;
    while ((current <= length(allFiles)) && (found == 0))
        if strcmp(allFiles(current).name, rufMainGUIData.Batch)
            found = current;
        end
        current = current + 1;
    end
    found = found + 1;
    if found > length(allFiles)
        found = 1;
    end
    rufMainGUIData.Batch = allFiles(found).name;
    rufBatchSelectionThumbnails(handles);


% --- Executes during object creation, after setting all properties.
function CurrentDirectoryText_CreateFcn(hObject, eventdata, handles)
% hObject    handle to CurrentDirectoryText (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called


% --- Executes on button press in SelectButton.
function SelectButton_Callback(hObject, eventdata, handles)
% hObject    handle to SelectButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufBatchSelectionGUI);


% --- Executes on button press in NewBatchButton.
function NewBatchButton_Callback(hObject, eventdata, handles)
% hObject    handle to NewBatchButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    directory = rufPatientRawDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.PatientID);
    allFiles = dir(strcat(directory, '/batch-*'));
    last = allFiles(length(allFiles));
    stringNumber = regexp(last.name, '\d*', 'match');
    newNumber = eval(cell2mat(stringNumber));
    newNumber = newNumber + 1;
    batchName = sprintf('batch-%03d', newNumber);
    mkdir(directory, batchName);
    rufMainGUIData.Batch = batchName;
    rufBatchSelectionThumbnails(handles);




% --- Executes on mouse press over axes background.
function Preview11_ButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to Preview11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)




% --- Executes during object creation, after setting all properties.
function Preview7_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Preview7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate Preview7


% --- Executes during object creation, after setting all properties.
function Preview1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Preview1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: place code in OpeningFcn to populate Preview1


