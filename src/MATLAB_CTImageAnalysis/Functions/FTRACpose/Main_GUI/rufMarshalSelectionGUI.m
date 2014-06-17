function varargout = rufMarshalSelectionGUI(varargin)
% RUFMARSHALSELECTIONGUI M-file for rufMarshalSelectionGUI.fig
%      RUFMARSHALSELECTIONGUI, by itself, creates a new RUFMARSHALSELECTIONGUI or raises the existing
%      singleton*.
%
%      H = RUFMARSHALSELECTIONGUI returns the handle to a new RUFMARSHALSELECTIONGUI or the handle to
%      the existing singleton*.
%
%      RUFMARSHALSELECTIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in RUFMARSHALSELECTIONGUI.M with the given input arguments.
%
%      RUFMARSHALSELECTIONGUI('Property','Value',...) creates a new RUFMARSHALSELECTIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before rufMarshalSelectionGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to rufMarshalSelectionGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help rufMarshalSelectionGUI

% Last Modified by GUIDE v2.5 28-Aug-2006 10:53:32

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @rufMarshalSelectionGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @rufMarshalSelectionGUI_OutputFcn, ...
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
end


% Refresh thumbnails
function numberOfSeeds = rufMarshalSelectionThumbnails(hObject, handles)
    global rufMainGUIData;
    allFiles = dir(strcat(handles.directory, '*-dewarp.*'));
    rufMainGUIData.FilesToSegment = allFiles;
    currentPreview = 1;
    currentFile = 1;
    
    numberOfSeeds = 0;
    
    while (currentPreview <= 12)
        
        % set current view
        currentAxes = eval(sprintf('handles.Preview%d', currentPreview));
        axes(currentAxes);

        % see if there are some files left
        if (currentFile <= length(allFiles))
            error = (1 == 0);
            % try to load
            try
                img = imread(strcat(handles.directory, allFiles(currentFile).name));
            catch
                error = (1 == 1);
            end
            % if loaded, verify that it was segmented
            if ~error
                if (rufSeedsAreSegmented(handles.directory, allFiles(currentFile).name) & rufPoseIsComputed(handles.directory, allFiles(currentFile).name))
                    % select and set current axes
                    colormap(gray);
                    imshow(img);
                    axis off;
                    % load seeds file to display number of seeds
                    [path, imagePrefix] = fileparts(allFiles(currentFile).name);
                    seedsFile = strcat(handles.directory, '/Seed_Segmentation/Seeds_', imagePrefix, '.txt');
                    seeds = load('-ascii', seedsFile);
                    nbSeeds = size(seeds);
                    nbSeeds = nbSeeds(1);
                    if nbSeeds > numberOfSeeds
                        numberOfSeeds = nbSeeds;
                    end
                    currentSelect = eval(sprintf('handles.Select%d', currentPreview));
                    set(currentSelect, 'Enable', 'on');
                    set(currentSelect, 'Value', 1);
                    set(currentSelect, 'String', strcat(imagePrefix, ' [', num2str(nbSeeds), ']'));
                    set(currentSelect, 'UserData', imagePrefix);
                    currentPreview = currentPreview + 1;
                end
            end
            % get ready for next image
            currentFile = currentFile + 1;
        else
            % no file left, clear axes
            colormap(gray);
            cla;
            axis off;
            currentSelect = eval(sprintf('handles.Select%d', currentPreview));
            set(currentSelect, 'Enable', 'off');
            set(currentSelect, 'Value', 0);
            set(currentSelect, 'String', 'No more file');
            currentPreview = currentPreview + 1;
        end
    end

end

    
% --- Executes just before rufMarshalSelectionGUI is made visible.
function rufMarshalSelectionGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to rufMarshalSelectionGUI (see VARARGIN)

% Choose default command line output for rufMarshalSelectionGUI
    handles.output = hObject;
    global rufMainGUIData;
    handles.directory = varargin{1};

    resultDir = strcat(handles.directory, '/3D_Seed_Locations');
    if ~isdir(resultDir)
        mkdir(resultDir)
    end
    
    % Update display 
    handles.numberOfSeeds = rufMarshalSelectionThumbnails(hObject, handles);
    set(handles.NbSeedsInput, 'String', num2str(handles.numberOfSeeds));

    % load camera calibration parameters
    calibImage = strcat(rufCalibrationProcessedDirectory(rufMainGUIData.RootDirectory, rufMainGUIData.CalibSession), rufMainGUIData.CalibImage);
    [pathstr, name, ext, versn] = fileparts(calibImage);
    filename = strcat(pathstr, '/', name, '-focal-length.txt');
    fileId = fopen(filename, 'rt');
    handles.focalLength = fscanf(fileId, '%f', 1);
    fclose(fileId);
    filename = strcat(pathstr, '/', name, '-pixel-size.txt');
    fileId = fopen(filename, 'rt');
    handles.pixelResolution = fscanf(fileId, '%f', 2);
    fclose(fileId);
    filename = strcat(pathstr, '/', name, '-origin.txt');
    fileId = fopen(filename, 'rt');
    handles.sourcePosition = fscanf(fileId, '%f', 2);
    fclose(fileId);
    
    % Update handles structure
    guidata(hObject, handles);
end

% UIWAIT makes rufMarshalSelectionGUI wait for user response (see UIRESUME)
% uiwait(handles.rufMarshalSelectionGUI);


% --- Outputs from this function are returned to the command line.
function varargout = rufMarshalSelectionGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
    varargout{1} = handles.output;
end

% --- Executes on button press in Done.
function Done_Callback(hObject, eventdata, handles)
% hObject    handle to Done (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    delete(handles.rufMarshalSelectionGUI);
end

% --- Executes on button press in RunMarshalButton.
function RunMarshalButton_Callback(hObject, eventdata, handles)
% hObject    handle to RunMarshalButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global rufMainGUIData;
    clear listOfFiles;
    currentFile = 1;
    selectedFile = 1;
    while (currentFile <= 12)
        selectHandle = eval(sprintf('handles.Select%d', currentFile));
        selected = get(selectHandle, 'Value');
        if (selected == 1)
            fileName = get(selectHandle, 'UserData');
            listOfFiles{selectedFile} = fileName;
            selectedFile = selectedFile + 1;
        end
        currentFile = currentFile + 1;
    end
    
    celldisp(listOfFiles);
    
    % camera parameters in format expected by Marshal
    % camera.ratio = [1 / handles.pixelResolution(1), 1 / handles.pixelResolution(2)];
    camera.ratio = handles.pixelResolution;
    camera.origin = handles.sourcePosition';
    % THE FOCAL LENGTH IS NEGATIVE HERE
    camera.focal = -handles.focalLength;
 
    % create vertical cell arrays as required by marshal
    nbImages = length(listOfFiles);
    segmentedSeeds = cell(nbImages, 1);
    poses = cell(nbImages, 1);
    cameras = cell(nbImages, 1);
    
    resultName = strcat(handles.directory, '/3D_Seed_Locations/Seeds_', rufMainGUIData.Batch);
    
    % loading 2d seeds and C-Arm pose for each image
    for n = 1:nbImages
        [pathstr, imagePrefix, ext, versn] = fileparts(listOfFiles{n});
        imagePrefixShort = regexprep(imagePrefix, '-dewarp', '');
 
        % build filename for result
        resultName = strcat(resultName, '_', imagePrefixShort);
 
        % seeds
        seedsFile = strcat(handles.directory, '/Seed_Segmentation/Seeds_', imagePrefix, '.txt');
        seeds = load('-ascii', seedsFile);
        segmentedSeeds{n} = seeds(:,1:2); % first two columns only
        
        % pose
        poseFile = strcat(handles.directory, '/Carm_Poses/Pose_', imagePrefix, '.txt');
        poses{n} = load('-ascii', poseFile);
        
        % camera, for now, same calibration for all poses
        cameras{n} = camera;
    end
    
    marshalInput = strcat(resultName, '.input');
    seeds3DName = strcat(resultName, '.txt');
    
    fid = fopen(marshalInput, 'wt');
    for n = 1:nbImages
       fprintf(fid, '%s\n', listOfFiles{n}); 
    end
    fclose(fid);
    
    [seeds3D, bpeAll, seeds2Dbp, matching, matchingInd] = marshal(segmentedSeeds, cameras, poses, handles.numberOfSeeds);
    
    % FTRAC to template transformation
    % this needs to be loaded from a file based on the template used
    % FTRAC2Template = 1.0e+002 * [ 0.00999986751438   0.00004610743392  -0.00002288666424   0.79564989338678
      %                            0.00004654340487  -0.00999800500951   0.00019424093629   1.54938460386802
       %                          -0.00002198650326  -0.00019434488520  -0.00999808715001  -1.15823210379216
        %                                         0                  0                  0   0.01000000000000];
        
    % new template 0 mm spacing
%     FTRAC2Template00 = 100 * [
%                                 0.00999894599888   0.00014306886136  -0.00002470247941   0.84976607411048
%                                 0.00014310104555  -0.00999896772883   0.00001290149907   1.60165130019690
%                                 -0.00002451534917  -0.00001325363432  -0.00999996116687  -1.07533259708400
%                                 0                  0                  0   0.01000000000000];
% 
%     % new template 15 mm spacing
%     FTRAC2Template15 = 100 * [  0.00999870928711   0.00015869806795  -0.00002505024968   0.84972469697709
%                                 0.00015869497997  -0.00999874061347  -0.00000143101433   1.75264844653388
%                                 -0.00002506980481   0.00000103329474  -0.00999996852181  -1.07646848058777
%                                 0                  0                  0   0.01000000000000];
% 
%     % new template 30mm spacing
%     FTRAC2Template30 = 100 * [                 
%                                0.00999903116621   0.00013788075547  -0.00001909540533   0.84980840566373
%                                0.00013788516782  -0.00999904910098   0.00000218096154   1.90150877796107
%                               -0.00001906351829  -0.00000244404756  -0.00999998153043  -1.07618030659338
%                                               0                  0                  0   0.01000000000000];

    % define which one is actually used
    FTRAC2Template = rufMainGUIData.FTRAC2Template;
    
    % compute actual positions in template frame
    new3Dpts = FTRAC2Template * [seeds3D'; ones(1, handles.numberOfSeeds)];
    new3Dpts = new3Dpts(1:3,:)';
    save(seeds3DName, 'new3Dpts', '-ascii');
    
    % save back projections for later
    for n = 1:nbImages
        [pathstr, imagePrefix, ext, versn] = fileparts(listOfFiles{n});
        bpName = strcat(resultName, '_bpe_', imagePrefix, '.txt');
        seeds2DBpThisImage = seeds2Dbp{n};
        save(bpName, 'seeds2DBpThisImage', '-ascii');
    end

    % inform user
    msgbox('Seed matching completed');
    
end


% --- Executes on button press in Select1.
function Select1_Callback(hObject, eventdata, handles)
% hObject    handle to Select1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select1
end

% --- Executes on button press in Select2.
function Select2_Callback(hObject, eventdata, handles)
% hObject    handle to Select2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select2
end

% --- Executes on button press in Select3.
function Select3_Callback(hObject, eventdata, handles)
% hObject    handle to Select3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select3
end

% --- Executes on button press in Select4.
function Select4_Callback(hObject, eventdata, handles)
% hObject    handle to Select4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select4
end

% --- Executes on button press in Select5.
function Select5_Callback(hObject, eventdata, handles)
% hObject    handle to Select5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select5
end

% --- Executes on button press in Select6.
function Select6_Callback(hObject, eventdata, handles)
% hObject    handle to Select6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select6
end

% --- Executes on button press in Select7.
function Select7_Callback(hObject, eventdata, handles)
% hObject    handle to Select7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select7
end

% --- Executes on button press in Select8.
function Select8_Callback(hObject, eventdata, handles)
% hObject    handle to Select8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select8
end

% --- Executes on button press in Select9.
function Select9_Callback(hObject, eventdata, handles)
% hObject    handle to Select9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select9
end

% --- Executes on button press in Select10.
function Select10_Callback(hObject, eventdata, handles)
% hObject    handle to Select10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select10
end

% --- Executes on button press in Select11.
function Select11_Callback(hObject, eventdata, handles)
% hObject    handle to Select11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select11
end

% --- Executes on button press in Select12.
function Select12_Callback(hObject, eventdata, handles)
% hObject    handle to Select12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Select12
end




function NbSeedsInput_Callback(hObject, eventdata, handles)
% hObject    handle to NbSeedsInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of NbSeedsInput as text
%        str2double(get(hObject,'String')) returns contents of NbSeedsInput as a double
    handles.numberOfSeeds = round(str2double(get(hObject,'String')));
    % sync data
    guidata(hObject, handles);
end 
    

% --- Executes during object creation, after setting all properties.
function NbSeedsInput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to NbSeedsInput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
    if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
        set(hObject,'BackgroundColor','white');
    end
end

