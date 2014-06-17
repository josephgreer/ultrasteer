function varargout = rufSegmentationSelectionGUI(varargin)
% RUFSEGMENTATIONSELECTIONGUI M-file for rufSegmentationSelectionGUI.fig
%      RUFSEGMENTATIONSELECTIONGUI, by itself, creates a new RUFSEGMENTATIONSELECTIONGUI or raises the existing
%      singleton*.
%
%      H = RUFSEGMENTATIONSELECTIONGUI returns the handle to a new RUFSEGMENTATIONSELECTIONGUI or the handle to
%      the existing singleton*.
%
%      RUFSEGMENTATIONSELECTIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFSEGMENTATIONSELECTIONGUI.M with the given input arguments.
%
%      RUFSEGMENTATIONSELECTIONGUI('Property','Value',...) creates a new RUFSEGMENTATIONSELECTIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufSegmentationSelectionGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufSegmentationSelectionGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufSegmentationSelectionGUI

% Last Modified by GUIDE v2.5 10-Jan-2007 12:31:15

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufSegmentationSelectionGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufSegmentationSelectionGUI_OutputFcn, ...
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
function rufSegmentationSelectionThumbnails(handles)
    global rufMainGUIData;
    allFiles = dir(strcat(handles.directory, '*-dewarp.*'));
    rufMainGUIData.FilesToSegment = allFiles;
    currentPreview = 1;
    currentFile = 1;
    while (currentPreview <= 12)
        
        % set current view
        currentAxes = eval(sprintf('handles.Preview%d', currentPreview));
        axes(currentAxes);

        % see if there are some files left
        if (currentFile <= length(allFiles))
            error = (1 == 0);
            fileName = allFiles(currentFile).name;
            % try to load
            try
                image = imread(strcat(handles.directory, fileName));
            catch
                error = (1 == 1);
            end
            % if loaded, display
            if ~error
                % select and set current axes
                colormap(gray);
                imshow(image, 'DisplayRange', []);
                axis off;
                % look for segmentation results
                currentStatus = eval(sprintf('handles.Status%d', currentPreview));
                if rufIsSegmented(handles.directory, fileName)
                    set(currentStatus, 'String', strcat(fileName, ' segmented'));
                else
                    set(currentStatus, 'String', strcat(fileName, ' not segmented'));
                end
                % go to next
                currentPreview = currentPreview + 1;
            end
            % get ready for next image
            currentFile = currentFile + 1;
        else
            % no file left, clear axes
            colormap(gray);
            cla;
            axis off;
            currentButton = eval(sprintf('handles.Seed%d', currentPreview));
            set(currentButton, 'Enable', 'off');
            currentButton = eval(sprintf('handles.Fiducial%d', currentPreview));
            set(currentButton, 'Enable', 'off');
            currentStatus = eval(sprintf('handles.Status%d', currentPreview));
            set(currentStatus, 'String', 'No more file');
            currentPreview = currentPreview + 1;
        end
    end

    
    

function SeedSegmentFunction(handles, imageNumber)
    global rufMainGUIData;
    imageName = rufMainGUIData.FilesToSegment(imageNumber).name;
    directory = handles.directory;
    image = imread(strcat(directory, imageName));
    [pathstr, imagePrefix, ext, versn] = fileparts(imageName);
    
    seedsFile = strcat(handles.directory, '/Seed_Segmentation/Seeds_', imagePrefix, '.txt');
    
    if (length(dir(seedsFile)) == 1)
        seeds = load('-ascii', seedsFile);
    else
        seeds = [];
    end
    
    % look at the size to deduct the resolution.  This is a last minute
    % hack.  By default, we have been using NTSC videos, ie. 480 x 480
    % usable field of view.  The resolution ratio is compared to NTSC.
    xySize = size(image);
    resRatioToNTSC = xySize(1) / 480;
    
    [handle SegmentationCompleted seeds] = SeedSegmentationGUI(image, seeds, resRatioToNTSC);
    if (~SegmentationCompleted)
        % errordlg('Seed segmentation aborted by user');
    else
        % test if all required directories exist
        newDirectory = strcat(directory, '/Seed_Segmentation');
        if ~isdir(newDirectory)
            mkdir(newDirectory)
        end
        
        % save the segmentation results
        save(seedsFile, 'seeds', '-ascii' );
        % refresh view
        rufSegmentationSelectionThumbnails(handles);
    end


function FiducialSegmentFunction(handles, imageNumber)
    global rufMainGUIData;
    imageName = rufMainGUIData.FilesToSegment(imageNumber).name;
    directory = handles.directory;
    image = imread(strcat(directory, imageName));
    image = image(:,:,1);           %% ANN ADDED becauase my images are 3D
    [pathstr, imagePrefix, ext, versn] = fileparts(imageName);
    
    % look at the size to deduct the resolution.  This is a last minute
    % hack.  By default, we have been using NTSC videos, ie. 480 x 480
    % usable field of view.  The resolution ratio is compared to NTSC.
    xySize = size(image);
    resRatioToNTSC = xySize(1) / 480;
    
    bbsFile = strcat(handles.directory, '/FTRAC_Segmentation/BBs_', imagePrefix, '.txt');  
    if (length(dir(bbsFile)) == 1)
        bbs = load('-ascii', bbsFile);
    else
        bbs = [];
    end
    line1File = strcat(handles.directory, '/FTRAC_Segmentation/Line1_', imagePrefix, '.txt');  
    if (length(dir(line1File)) == 1)
        line1 = load('-ascii', line1File);
    else
        line1 = [];
    end
    line2File = strcat(handles.directory, '/FTRAC_Segmentation/Line2_', imagePrefix, '.txt');  
    if (length(dir(line2File)) == 1)
        line2 = load('-ascii', line2File);
    else
        line2 = [];
    end
    line3File = strcat(handles.directory, '/FTRAC_Segmentation/Line3_', imagePrefix, '.txt');  
    if (length(dir(line3File)) == 1)
        line3 = load('-ascii', line3File);
    else
        line3 = [];
    end
    ellipse1File = strcat(handles.directory, '/FTRAC_Segmentation/Ellipse1_', imagePrefix, '.txt');  
    if (length(dir(ellipse1File)) == 1)
        ellipse1 = load('-ascii', ellipse1File);
    else
        ellipse1 = [];
    end
    ellipse2File = strcat(handles.directory, '/FTRAC_Segmentation/Ellipse2_', imagePrefix, '.txt');  
    if (length(dir(ellipse2File)) == 1)
        ellipse2 = load('-ascii', ellipse2File);
    else
        ellipse2 = [];
    end
    
    [handle SegmentationCompleted bbs line1 line2 line3 ellipse1 ellipse2] = FiducialSegmentationGUI(image, bbs, ...
                                                                                                     line1, line2, line3, ...
                                                                                                     ellipse1, ellipse2, ...
                                                                                                     resRatioToNTSC);
    if (~SegmentationCompleted)
        % errordlg('Fiducial segmentation aborted by user');
    else
        % test if all required directories exist
        newDirectory = strcat(directory, '/FTRAC_Segmentation');
        if ~isdir(newDirectory)
            mkdir(newDirectory)
        end
        newDirectory = strcat(directory, '/Carm_Poses');
        if ~isdir(newDirectory)
            mkdir(newDirectory)
        end
        
        % save the segmentation results
        save(strcat(directory, '/FTRAC_Segmentation/BBs_', imagePrefix, '.txt'), 'bbs', '-ascii' );
        save(strcat(directory, '/FTRAC_Segmentation/Line1_', imagePrefix, '.txt'), 'line1', '-ascii' );
        save(strcat(directory, '/FTRAC_Segmentation/Line2_', imagePrefix, '.txt'), 'line2', '-ascii' );
        save(strcat(directory, '/FTRAC_Segmentation/Line3_', imagePrefix, '.txt'), 'line3', '-ascii' );
        save(strcat(directory, '/FTRAC_Segmentation/Ellipse1_', imagePrefix, '.txt'), 'ellipse1', '-ascii' );
        save(strcat(directory, '/FTRAC_Segmentation/Ellipse2_', imagePrefix, '.txt'), 'ellipse2', '-ascii' );

        % refresh view
        rufSegmentationSelectionThumbnails(handles);
    end
    
    
    
% --- Executes just before rufSegmentationSelectionGUI is made visible.
function rufSegmentationSelectionGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufSegmentationSelectionGUI (see VARARGIN)

% Choose default command line output for rufSegmentationSelectionGUI
    handles.output = hObject;
    global rufMainGUIData;

    handles.directory = varargin{1};
    rufSegmentationSelectionThumbnails(handles);

    % Update handles structure
    guidata(hObject, handles);

% UIWAIT makes rufSegmentationSelectionGUI wait for user response (see UIRESUME)
% uiwait(handles.rufSegmentationSelectionGUI);




% --- Outputs from this function are returned to the command line.
function varargout = rufSegmentationSelectionGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;




% --- Executes on button press in Done.
function Done_Callback(hObject, eventdata, handles)
% hObject    handle to Done (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufSegmentationSelectionGUI);


% --- Executes on button press in Fiducial1.
function Fiducial1_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 1);

% --- Executes on button press in Seed1.
function Seed1_Callback(hObject, eventdata, handles)
% hObject    handle to Seed1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 1);   

% --- Executes on button press in Fiducial2.
function Fiducial2_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 2);

% --- Executes on button press in Seed2.
function Seed2_Callback(hObject, eventdata, handles)
% hObject    handle to Seed2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 2);   

% --- Executes on button press in Fiducial3.
function Fiducial3_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 3);

% --- Executes on button press in Seed3.
function Seed3_Callback(hObject, eventdata, handles)
% hObject    handle to Seed3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 3);   

% --- Executes on button press in Fiducial4.
function Fiducial4_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 4);

% --- Executes on button press in Seed4.
function Seed4_Callback(hObject, eventdata, handles)
% hObject    handle to Seed4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 4);   

% --- Executes on button press in Fiducial5.
function Fiducial5_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 5);

% --- Executes on button press in Seed5.
function Seed5_Callback(hObject, eventdata, handles)
% hObject    handle to Seed5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 5);   

% --- Executes on button press in Fiducial6.
function Fiducial6_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 6);

% --- Executes on button press in Seed6.
function Seed6_Callback(hObject, eventdata, handles)
% hObject    handle to Seed6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 6);   

% --- Executes on button press in Fiducial7.
function Fiducial7_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 7);

% --- Executes on button press in Seed7.
function Seed7_Callback(hObject, eventdata, handles)
% hObject    handle to Seed7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 7);   

% --- Executes on button press in Fiducial8.
function Fiducial8_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 8);

% --- Executes on button press in Seed8.
function Seed8_Callback(hObject, eventdata, handles)
% hObject    handle to Seed8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 8);   

% --- Executes on button press in Fiducial9.
function Fiducial9_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 9);

% --- Executes on button press in Seed9.
function Seed9_Callback(hObject, eventdata, handles)
% hObject    handle to Seed9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 9);   

% --- Executes on button press in Fiducial10.
function Fiducial10_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 10);

% --- Executes on button press in Seed10.
function Seed10_Callback(hObject, eventdata, handles)
% hObject    handle to Seed10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 10);   

% --- Executes on button press in Fiducial11.
function Fiducial11_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 11);

% --- Executes on button press in Seed11.
function Seed11_Callback(hObject, eventdata, handles)
% hObject    handle to Seed11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 11);   

% --- Executes on button press in Fiducial12.
function Fiducial12_Callback(hObject, eventdata, handles)
% hObject    handle to Fiducial12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    FiducialSegmentFunction(handles, 12);

% --- Executes on button press in Seed12.
function Seed12_Callback(hObject, eventdata, handles)
% hObject    handle to Seed12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    SeedSegmentFunction(handles, 12);   

