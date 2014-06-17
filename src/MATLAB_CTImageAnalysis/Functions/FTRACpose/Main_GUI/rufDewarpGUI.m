function varargout = rufDewarpGUI(varargin)
% RUFDEWARPGUI M-file for rufDewarpGUI.fig
%      RUFDEWARPGUI, by itself, creates a new RUFDEWARPGUI or raises the existing
%      singleton*.
%
%      H = RUFDEWARPGUI returns the handle to a new RUFDEWARPGUI or the handle to
%      the existing singleton*.
%
%      RUFDEWARPGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFDEWARPGUI.M with the given input arguments.
%
%      RUFDEWARPGUI('Property','Value',...) creates a new RUFDEWARPGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufDewarpGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufDewarpGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufDewarpGUI

% Last Modified by GUIDE v2.5 22-Sep-2006 17:49:44

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufDewarpGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufDewarpGUI_OutputFcn, ...
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


% --- Executes just before rufDewarpGUI is made visible.
function rufDewarpGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufDewarpGUI (see VARARGIN)

% Choose default command line output for rufDewarpGUI
handles.output = hObject;

    global rufMainGUIData;

    handles.inputDirectory = varargin{1};
    handles.outputDirectory = varargin{2};
    
    if ~isdir(handles.outputDirectory)
        mkdir(handles.outputDirectory);
    end;
    
    % clear images
    axes(handles.OriginalAxes);
    colormap(gray);
    axis off;    
    cla;
    axes(handles.ResultAxes);
    colormap(gray);
    axis off;    
    cla;
    
    % find and display info used
    calibImage = strcat(rufCalibrationProcessedDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.CalibSession), rufMainGUIData.CalibImage);
    [pathstr, name, ext, versn] = fileparts(calibImage);
    handles.coeffFile = strcat(pathstr, '/', name, '-map-coords.txt');
    handles.maskFile =  strcat(pathstr, '/', name, '-mask.tiff');
    handles.mask = double(imread(handles.maskFile));
    handles.mask = handles.mask ./ max(max(handles.mask));
    set(handles.CoeffText, 'string', handles.coeffFile);
    
    % Update handles structure
    guidata(hObject, handles);

% UIWAIT makes rufDewarpGUI wait for user response (see UIRESUME)
% uiwait(handles.rufDewarpGUI);


% --- Outputs from this function are returned to the command line.
function varargout = rufDewarpGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in DewarpButton.
function DewarpButton_Callback(hObject, eventdata, handles)
% hObject    handle to DewarpButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    % Find files to process
    allFiles = dir(strcat(handles.inputDirectory, '/img-*.tiff'));
    nbFiles = length(allFiles);
    
    
    if nbFiles == 0
        warnHandle = warndlg('The input directory is empty','Warning');
        uiwait(warnHandle);
    else
        try
            % Read the coordinates from the file
            [Xcoords, Ycoords] = ReadMappedCoordsFrmFile(handles.coeffFile);
    
            for index = 1:nbFiles
                inputFile = strcat(handles.inputDirectory, '/', allFiles(index).name);
                [pathstr, name, extension, versn] = fileparts(inputFile);
                maskedFile = strcat(handles.outputDirectory, name, '-masked.tiff');
                dewarpFile = strcat(handles.outputDirectory, name, '-dewarp.tiff');
                % load the image as integers and convert using max contrast
                inputImage = imread(inputFile);
                doubleImage = double(inputImage);
                maxValue = max(max(doubleImage));
                doubleImage = doubleImage ./ maxValue;
                % Apply mask and save masked image for trace
                maskedImage = doubleImage .* handles.mask;
                imwrite(maskedImage, maskedFile, 'tiff');
                % Dewarp and save
                [dewarpImage] = DewarpImageFromMappedCoordinatesLoaded(Xcoords, Ycoords, maskedImage);
                imwrite(dewarpImage, dewarpFile, 'tiff');

                % display latest results
                axes(handles.OriginalAxes);
                axis off;    
                imshow(inputImage, 'DisplayRange', []);
                axes(handles.ResultAxes);
                axis off;    
                imshow(dewarpImage, 'DisplayRange', []);
                set(handles.CurrentImageText, 'string', dewarpFile);
                drawnow;
            end
            warnHandle = warndlg(strcat(num2str(nbFiles), ' image(s) dewarped'),'Information');
            uiwait(warnHandle);
        catch
            err = lasterror;
            disp('--- Dewarping Error message ---');
            disp(err.message);
            set(gcf,'Pointer','arrow');
            drawnow;
            errordlg('Can not dewarp, make sure the calibration date matches the study','Dewarping Error')
        end
    end
    
    % show we are done
    set(gcf,'Pointer','arrow');
    drawnow;
    



% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufDewarpGUI);

