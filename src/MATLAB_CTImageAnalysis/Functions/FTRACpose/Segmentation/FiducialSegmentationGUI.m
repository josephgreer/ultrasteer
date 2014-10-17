% $Id: FiducialSegmentationGUI.m,v 1.10 2007/09/19 14:50:17 anton Exp $

function varargout = FiducialSegmentationGUI(varargin)
% FIDUCIALSEGMENTATIONGUI M-file for FiducialSegmentationGUI.fig
%      FIDUCIALSEGMENTATIONGUI, by itself, creates a new FIDUCIALSEGMENTATIONGUI or raises the existing
%      singleton*.
%
%      H = FIDUCIALSEGMENTATIONGUI returns the handle to a new FIDUCIALSEGMENTATIONGUI or the handle to
%      the existing singleton*.
%
%      FIDUCIALSEGMENTATIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in FIDUCIALSEGMENTATIONGUI.M with the given input arguments.
%
%      FIDUCIALSEGMENTATIONGUI('Property','Value',...) creates a new FIDUCIALSEGMENTATIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before FiducialSegmentationGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to FiducialSegmentationGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help FiducialSegmentationGUI

% Last Modified by GUIDE v2.5 02-Aug-2007 16:45:48

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @FiducialSegmentationGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @FiducialSegmentationGUI_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
                              

if nargin & isstr(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before FiducialSegmentationGUI is made visible.
function FiducialSegmentationGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to FiducialSegmentationGUI (see VARARGIN)

%Clearing the global variables used for output. This way values from the
%previous run will not be carried over. Added by Ameet.
    clear global FiducialSegmentation;
    global FiducialSegmentation;
    
    FiducialSegmentation.Complete = false;
    
    handles.output = hObject;
    
    % open the file, display it
    [FiducialSegmentation.I] = varargin{1};
    [FiducialSegmentation.BBs] = varargin{2};
    if isempty(FiducialSegmentation.BBs)
        FiducialSegmentation.BBs = -1 * ones(9, 2);
    end
    [FiducialSegmentation.Line1] = varargin{3};
    [FiducialSegmentation.Line2] = varargin{4};
    [FiducialSegmentation.Line3] = varargin{5};
    [FiducialSegmentation.Ellipse1] = varargin{6};
    [FiducialSegmentation.Ellipse2] = varargin{7};
    FiducialSegmentation.ResRatioToNTSC = varargin{8};
    
    FiducialSegmentation.PointsReqEllipse = 7;
    FiducialSegmentation.PointsReqLine = 5;
    FiducialSegmentation.UserPoints = [];
    
    [rows,cols] = size(FiducialSegmentation.I); 
    FiducialSegmentation.ROI.StartX = 1;
    FiducialSegmentation.ROI.StartY = 1;
    FiducialSegmentation.ROI.SizeX = rows - 1;
    FiducialSegmentation.ROI.SizeY = cols - 1;
    axes(handles.ImageDisplay);
    DisplayFiducialSegmentation();
    UpdateStatus(hObject, handles);

    % Update handles structure
    guidata(hObject, handles);



function UnToggleAllButtons(handles)
    global FiducialSegmentation;
    FiducialSegmentation.TaskIdentifier = 0;
    FiducialSegmentation.UserPoints = [];
    set(handles.AddBBToggleButton, 'Value', 0);
    set(handles.AddBBToggleButton, 'BackgroundColor', 'default');
    set(handles.DeleteBBToggleButton, 'Value', 0);
    set(handles.DeleteBBToggleButton, 'BackgroundColor', 'default');
    set(handles.AddLineToggleButton, 'Value', 0);
    set(handles.AddLineToggleButton, 'BackgroundColor', 'default');
    set(handles.DeleteLineToggleButton, 'Value', 0);
    set(handles.DeleteLineToggleButton, 'BackgroundColor', 'default');
    set(handles.AddEllipseToggleButton, 'Value', 0);
    set(handles.AddEllipseToggleButton, 'BackgroundColor', 'default');
    set(handles.DeleteEllipseToggleButton, 'Value', 0);
    set(handles.DeleteEllipseToggleButton, 'BackgroundColor', 'default');
    set(handles.UserAlertMsg, 'String', '');    
    set(gcf, 'pointer', 'arrow');



function UpdateStatus(hObject, handles)
    global FiducialSegmentation;
    goodBBs = sum(FiducialSegmentation.BBs(: ,1) > 0);
    message{1} = strcat('BBs: ', num2str(goodBBs));
    if ~isempty(FiducialSegmentation.Line1)
        message{2} = 'Line 1: Defined';
    else
        message{2} = 'Line 1: Undefined';
    end
    if ~isempty(FiducialSegmentation.Line2)
        message{3} = 'Line 2: Defined';
    else
        message{3} = 'Line 2: Undefined';
    end
    if ~isempty(FiducialSegmentation.Line3)
        message{4} = 'Line 3: Defined';
    else
        message{4} = 'Line 3: Undefined';
    end
    if ~isempty(FiducialSegmentation.Ellipse1)
        message{5} = 'Ellipse 1: Defined';
    else
        message{5} = 'Ellipse 1: Undefined';
    end
    if ~isempty(FiducialSegmentation.Ellipse2)
        message{6} = 'Ellipse 2: Defined';
    else
        message{6} = 'Ellipse 2: Undefined';
    end
    message{7} = strcat('Points: ', num2str(size(FiducialSegmentation.UserPoints, 1)));
    
    set(handles.StatusText, 'String', message);
    % Update handles structure
    guidata(hObject, handles);
   
    
    

% --- Outputs from this function are returned to the command line.
function varargout = FiducialSegmentationGUI_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure

%Uiwait and changed output inserted by by Ameet % REMOVED BY ANN
    global FiducialSegmentation;

    uiwait(handles.output)

    varargout{1} = handles.output;
    varargout{2} = FiducialSegmentation.Complete;
    if (FiducialSegmentation.Complete == false)
        for tmp_counter = 3 : 8
            varargout{tmp_counter} = [];
        end
    else   
        varargout{3} = FiducialSegmentation.BBs;
        varargout{4} = FiducialSegmentation.Line1;
        varargout{5} = FiducialSegmentation.Line2;
        varargout{6} = FiducialSegmentation.Line3;
        varargout{7} = FiducialSegmentation.Ellipse1;
        varargout{8} = FiducialSegmentation.Ellipse2;
    end






% //-----------------------------------------------------------------------------
% // function:	ZoomToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Select region button is clicked
%                   on the GUI in fiducial segmentation group of buttons, 
%                   allows the user to select region of
%                   interest where the fiducial, this function
%                   further calls fiucial segmentation routines to perform the
%                   segmentation, display routine to display the results,
%                   after completeion of segmentation also activates
%                   further buttons to enable the user to accept the
%                   segmentation results or make changes
% // input:	global variable I (image buffer)
% // output: global variables: 
% variables that identify the region of interest coordinates
%           global FiducialROIStartX; 
%           global FiducialROIStartY; 
%           global FiducialROIXWidth; 
%           global FiducialROIYWidth;
%           global FiducialSegmentationROITranslationVector;
% variables in which the results will be put
% variable for BB Locations
%             global BBLocations;
% variable for Line equations
%             global lLine1PointsEq;
%             global lLine2PointsEq;
%             global lLine3PointsEq;
% variable for ellipse points
%             global lEllipse1Points; 
%             global lEllipse2Points;
% variables which are floating buffers for the results, which are modified
% when user performs Add or Delete operations for BBs, Lines, Ellipses, 
% all the changes are made to these buffers rather than original, 
% it is copied onto original when user clicks Accept button
%             global Floating_BBLocations;
%             global Floating_lLine1PointsEq;
%             global Floating_lLine2PointsEq;
%             global Floating_lLine3PointsEq; 
%             global Floating_lEllipse1Points;
%             global Floating_lEllipse2Points;         
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global FiducialUndoTaskList;
% variables useful to perform/maintain user changes 
%             global bLine1Removed; 
%             global bLine2Removed; 
%             global bLine3Removed;
%             global bEllipse1Removed; 
%             global bEllipse2Removed;
%             global NumOfPointsToDefineLine; 
%             global PointsOnLine;
%             global NumOfPointsToDefineEllipse; 
%             global PointsOnEllipse;
% // calling function: called when Select region button is pressed
% // NOTE: 
% //-----------------------------------------------------------------------------
% --- Executes on button press in ZoomToggleButton.
function ZoomToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to ZoomToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

    % global variable required:
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);

    if( button_state == get(hObject,'Max'))
    % toggle button is pressed
        % change the foreground color
        set(hObject, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'String', 'Zoom in: Select an area with the mouse');
        set(gcf, 'pointer', 'crosshair');
        
        % get  a rectangular region of interest
        rect = getrect(handles.ImageDisplay);
        % get the corners of the rectangle
        FiducialSegmentation.ROI.StartX = round(rect(1,2));
        FiducialSegmentation.ROI.StartY = round(rect(1,1));
        FiducialSegmentation.ROI.SizeX = round(rect(1,4));
        FiducialSegmentation.ROI.SizeY = round(rect(1,3));

        % display results only on region of interest, gives the zoom effect
        set(handles.UserAlertMsg, 'String', '');
        set(gcf, 'pointer', 'arrow');
        DisplayFiducialSegmentation;
        
    elseif( button_state == get(hObject,'Min'))
        % toggle button is not pressed
        % change the foreground color
        set(hObject, 'BackgroundColor', 'default');
        
        [rows,cols] = size(FiducialSegmentation.I);
        FiducialSegmentation.ROI.StartX = 1;
        FiducialSegmentation.ROI.StartY = 1;
        FiducialSegmentation.ROI.SizeX = rows - 1;
        FiducialSegmentation.ROI.SizeY = cols - 1;
        axes(handles.ImageDisplay);
        DisplayFiducialSegmentation;
    end

    % Update handles structure
    guidata(hObject, handles);








% //-----------------------------------------------------------------------------
% // function:	AddBBToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Add BB toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTask Identifier
%                   to 3 indicating the current task is to add BBs
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when AddBB toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in AddBBToggleButton.
function AddBBToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddBBToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddBBToggleButton
% manage the state/color of the add button
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.AddBBToggleButton, 'BackgroundColor', 'white');
        set(handles.AddBBToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 3;
        set(handles.UserAlertMsg, 'String', 'Add BB active: Click on image to add a BB');
        set(gcf, 'pointer', 'crosshair');
    end
    





% //-----------------------------------------------------------------------------
% // function:	DeleteBBToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Delete BB toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTask Identifier
%                   to 4 indicating the current task is to delete BBs
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Delete BB toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in DeleteBBToggleButton.
function DeleteBBToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteBBToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of DeleteBBToggleButton
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.DeleteBBToggleButton, 'BackgroundColor', 'white');
        set(handles.DeleteBBToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 4;
        set(handles.UserAlertMsg, 'String', 'Delete BB active: Click on image to add a BB');
        set(gcf, 'pointer', 'crosshair');
    end


    



% //-----------------------------------------------------------------------------
% // function:	AddLineToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Add Line toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 5 indicating the current task is to add Line
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Add line toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------
% ------
% --- Executes on button press in AddLineToggleButton.
function AddLineToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddLineToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddLineToggleButton
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.AddLineToggleButton, 'BackgroundColor', 'white');
        set(handles.AddLineToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 5;
        set(handles.UserAlertMsg, 'String', 'Add line active: Give 5 points on image to add a line');
        set(gcf, 'pointer', 'crosshair');
    end
 


    
    
    
% --- Executes on button press in DeleteAllButton.
function DeleteAllButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteAllButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FiducialSegmentation;
    FiducialSegmentation.BBs = -1 * ones(9, 2);
    FiducialSegmentation.Line1 = [];
    FiducialSegmentation.Line2 = [];
    FiducialSegmentation.Line3 = [];
    FiducialSegmentation.Ellipse1 = [];
    FiducialSegmentation.Ellipse2 = [];
    axes(handles.ImageDisplay);
    DisplayFiducialSegmentation;
    

% //-----------------------------------------------------------------------------
% // function:	DeleteLineToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Delete Line toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 6 indicating the current task is to delete Line
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Delete line toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in DeleteLineToggleButton.
function DeleteLineToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteLineToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of DeleteLineToggleButton
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.DeleteLineToggleButton, 'BackgroundColor', 'white');
        set(handles.DeleteLineToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 6;
        set(handles.UserAlertMsg, 'String', 'Delete line active: Click on image to delete a line');
        set(gcf, 'pointer', 'crosshair');
    end





% //-----------------------------------------------------------------------------
% // function:	AddEllipseToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Add ellipse toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 7 indicating the current task is to add ellipse
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Add ellipse toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in AddEllipseToggleButton.
function AddEllipseToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddEllipseToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddEllipseToggleButton
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.AddEllipseToggleButton, 'BackgroundColor', 'white');
        set(handles.AddEllipseToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 7;
        set(handles.UserAlertMsg, 'String', 'Add ellipse active: Give 7 points on image to add an ellipse');
        set(gcf, 'pointer', 'crosshair');
    end




% //-----------------------------------------------------------------------------
% // function:	DeleteEllipseToggleButton_Callback() 
% // file:		FiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Delete ellipse toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 8 indicating the current task is to delete ellipse
%                   wherever the user clicks until the state of the button
%                   is changed
% // input:	
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Delete ellipse toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in DeleteEllipseToggleButton.
function DeleteEllipseToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteEllipseToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of DeleteEllipseToggleButton
    % fiducial segmentation related global variables
    global FiducialSegmentation;
    
    % get the state and then untoggle everything
    button_state = get(hObject,'Value');
    UnToggleAllButtons(handles);
    
    % manage the state/color of the add button
    if( button_state == get(hObject,'Max'))
        % change the foreground color
        set(handles.DeleteEllipseToggleButton, 'BackgroundColor', 'white');
        set(handles.DeleteEllipseToggleButton, 'Value', get(hObject, 'Max'));
        FiducialSegmentation.TaskIdentifier = 8;
        set(handles.UserAlertMsg, 'String', 'Delete ellipse active: Click on image to delete an ellipse');
        set(gcf, 'pointer', 'crosshair');
    end






% --- Executes on button press in SwapEllipsesButton.
function SwapEllipsesButton_Callback(hObject, eventdata, handles)
% hObject    handle to SwapEllipsesButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FiducialSegmentation;
    tmp = FiducialSegmentation.Ellipse1;
    FiducialSegmentation.Ellipse1 = FiducialSegmentation.Ellipse2;
    FiducialSegmentation.Ellipse2 = tmp;
    axes(handles.ImageDisplay);
    DisplayFiducialSegmentation;




% --- Executes on button press in CancelButton.
function CancelButton_Callback(hObject, eventdata, handles)
% hObject    handle to CancelButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FiducialSegmentation;
    UnToggleAllButtons(handles);
    FiducialSegmentation.Complete = false;
    delete(handles.FiducialSegmentationGUI);

    
% --- Executes during object deletion, before destroying properties.
function FiducialSegmentationGUI_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to FiducialSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    


% --- Executes when user attempts to close FiducialSegmentationGUI.
function FiducialSegmentationGUI_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to FiducialSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure
    global FiducialSegmentation;
    UnToggleAllButtons(handles);
    FiducialSegmentation.Complete = false;
    delete(handles.FiducialSegmentationGUI);


% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% transform the results from Region Of Interest coordinate system to actual Image coordinate system 
% involves addition of translation vector before feeding the points to
% obtain/formulate final equations in image coordinate frame
% 1st the seeds locations
    global FiducialSegmentation;
    UnToggleAllButtons(handles);
    FiducialSegmentation.Complete = true;
    delete(handles.FiducialSegmentationGUI);



    
    
% --- Executes on button press in AutoSegmentBBsButton.
function AutoSegmentBBsButton_Callback(hObject, eventdata, handles)
% hObject    handle to AutoSegmentBBsButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Step 1; first perform the fiducial BB segmentation;
    global FiducialSegmentation;
    
    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    Xoffset = FiducialSegmentation.ROI.StartX - 1;
    Yoffset = FiducialSegmentation.ROI.StartY - 1;

    BBsROI = FiducialBBSegmentation(FiducialSegmentation.I(FiducialSegmentation.ROI.StartX:FiducialSegmentation.ROI.StartX + FiducialSegmentation.ROI.SizeX, ...
                                                           FiducialSegmentation.ROI.StartY:FiducialSegmentation.ROI.StartY + FiducialSegmentation.ROI.SizeY), ...
                                                           FiducialSegmentation.ResRatioToNTSC);

    % convert to global coordinate frame if valid
    for iBB = 1: length(FiducialSegmentation.BBs)
        if (BBsROI(iBB,1) ~= -1)
            FiducialSegmentation.BBs(iBB, 1) = BBsROI(iBB, 1) + Xoffset;
            FiducialSegmentation.BBs(iBB, 2) = BBsROI(iBB, 2) + Yoffset;
        else
            FiducialSegmentation.BBs(iBB, 1) = -1;
            FiducialSegmentation.BBs(iBB, 2) = -1;
        end
    end

    % display results
    axes(handles.ImageDisplay);
    DisplayFiducialSegmentation();
    UpdateStatus(hObject, handles);

    set(gcf, 'pointer', 'arrow');    
    set(handles.UserAlertMsg, 'string', '');
    drawnow;
    


% --- Executes on button press in AutoSegmentLineEllipsesButton.
function AutoSegmentLineEllipsesButton_Callback(hObject, eventdata, handles)
% hObject    handle to AutoSegmentLineEllipsesButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FiducialSegmentation;

    % show we are busy
    set(gcf,'Pointer','watch');
    drawnow;
    
    Xoffset = FiducialSegmentation.ROI.StartX - 1;
    Yoffset = FiducialSegmentation.ROI.StartY - 1;
    
    set(handles.UserAlertMsg, 'string', 'Performing fiducial segmentation...');
    set(gcf, 'pointer', 'watch');

    % FiducialSegmentation.I = histeq(FiducialSegmentation.I);
    
    % Step 1; first import BB segmentation
    % convert to global coordinate frame if valid
    for iBB = 1: length(FiducialSegmentation.BBs)
        if (FiducialSegmentation.BBs(iBB,1) ~= -1)
            BBsROI(iBB, 1) = FiducialSegmentation.BBs(iBB, 1) - Xoffset;
            BBsROI(iBB, 2) = FiducialSegmentation.BBs(iBB, 2) - Yoffset;
        else
            BBsROI(iBB, 1) = -1;
            BBsROI(iBB, 2) = -1;
        end
    end
    
    % % Step 2: form an edge image
    IROIFiducial = double(FiducialSegmentation.I(FiducialSegmentation.ROI.StartX:FiducialSegmentation.ROI.StartX + FiducialSegmentation.ROI.SizeX, FiducialSegmentation.ROI.StartY:FiducialSegmentation.ROI.StartY + FiducialSegmentation.ROI.SizeY));
    InegROIFiducial = double(imcomplement(FiducialSegmentation.I(FiducialSegmentation.ROI.StartX:FiducialSegmentation.ROI.StartX + FiducialSegmentation.ROI.SizeX, FiducialSegmentation.ROI.StartY:FiducialSegmentation.ROI.StartY + FiducialSegmentation.ROI.SizeY)));

    FiducialEdgeImage = [];
    FiducialEdgeImage = BuildEdgeImage(InegROIFiducial,2);
    X_Dimension = length(FiducialEdgeImage(:,1));
    Y_Dimension = length(FiducialEdgeImage(1,:));

    % Step 3: perform line detection by graph search
    
    % first locate line's estimated start and end points from corresponding BBs located
    % locate the estimated point from the earlier found BB locations
    [Line1StartingPoint, Line1EndPoint, ...
     Line2StartingPoint, Line2EndPoint, ...
     Line3StartingPoint, Line3EndPoint] = GetLineStartAndEndPointsFromBBs(BBsROI, ...
                                                                          X_Dimension, ...
                                                                          FiducialSegmentation.ResRatioToNTSC);
    
    % find direction estimate if both start and end points are available for any line
    [UseDirectionEstimate InitialDirectionEstimate] = CalculateLineDirectionEstimateFromEndPoints(Line1StartingPoint, Line1EndPoint, ...
                                                                                                  Line2StartingPoint, Line2EndPoint, ...
                                                                                                  Line3StartingPoint, Line3EndPoint, ...
                                                                                                  X_Dimension, Y_Dimension);

    % 1st line
    if (length(Line1StartingPoint) > 1 || length(Line1EndPoint) > 1)
        [Line1Found Line1ROI StartPointValid EndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,...
                                                                          Line1StartingPoint, Line1EndPoint, ...
                                                                          UseDirectionEstimate, InitialDirectionEstimate);
        if(~StartPointValid)
            Line1StartingPoint(1,1) = -1;        
            Line1StartingPoint(1,2) = -1;
        end
        if(~EndPointValid)
            Line1EndPoint(1,1) = -1;
            Line1EndPoint(1,2) = -1;
        end
    end
    if ~Line1Found
        Line1ROI = [];
    end

% % 2nd line
    if (length(Line2StartingPoint) > 1 || length(Line2EndPoint) > 1)
        [Line2Found Line2ROI StartPointValid EndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,...
            Line2StartingPoint, Line2EndPoint, UseDirectionEstimate, InitialDirectionEstimate);
        if(~StartPointValid)
            Line2StartingPoint(1,1) = -1;        
            Line2StartingPoint(1,2) = -1;
        end

        if(~EndPointValid)
            Line2EndPoint(1,1) = -1;
            Line2EndPoint(1,2) = -1;
        end
    end
    if ~Line2Found
        Line2ROI = [];
    end

% % 3rd line
    if (length(Line3StartingPoint) > 1 || length(Line3EndPoint) > 1)
        [Line3Found Line3ROI StartPointValid EndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,...
            Line3StartingPoint, Line3EndPoint, UseDirectionEstimate, InitialDirectionEstimate);
        if(StartPointValid)
            Line3StartingPoint(1,1) = -1;        
            Line3StartingPoint(1,2) = -1;
        end
        if(EndPointValid)
            Line3EndPoint(1,1) = -1;
            Line3EndPoint(1,2) = -1;
        end
    end
    if ~Line3Found
        Line3ROI = [];
    end

    % recalculate BB locations using line equations & establish corespondence
    [BBsROI Line1ROI Line2ROI Line3ROI Line1Found Line2Found Line3Found] = RecalculateBBLocationsAndEstablishCorrespondence(IROIFiducial, BBsROI,...
            Line1ROI, Line2ROI, Line3ROI,...
            Line1Found, Line2Found, Line3Found,...
            Line1StartingPoint, Line2StartingPoint, Line3StartingPoint);

    % ellipse detection
    [Ellipse1ROI Ellipse2ROI] =  NewExtractEllipses(InegROIFiducial, Line2ROI, Line1ROI, Line3ROI, FiducialSegmentation.ResRatioToNTSC);
    
    % convert to global coordinate frame if valid
    for iBB = 1: length(FiducialSegmentation.BBs)
        if (BBsROI(iBB,1) ~= -1)
            FiducialSegmentation.BBs(iBB, 1) = BBsROI(iBB, 1) + Xoffset;
            FiducialSegmentation.BBs(iBB, 2) = BBsROI(iBB, 2) + Yoffset;
        else
            FiducialSegmentation.BBs(iBB, 1) = -1;
            FiducialSegmentation.BBs(iBB, 2) = -1;
        end
    end

    if Line1Found
        FiducialSegmentation.Line1 = Line1ROI;
        FiducialSegmentation.Line1(3) = FiducialSegmentation.Line1(3) - FiducialSegmentation.Line1(1) * Xoffset - FiducialSegmentation.Line1(2) * Yoffset;
    else
        FiducialSegmentation.Line1 = [];
    end
    
    if Line2Found
        FiducialSegmentation.Line2 = Line2ROI;
        FiducialSegmentation.Line2(3) = FiducialSegmentation.Line2(3) - FiducialSegmentation.Line2(1) * Xoffset - FiducialSegmentation.Line2(2) * Yoffset;       
    else
        FiducialSegmentation.Line2 = [];
    end
    
    if Line3Found
        FiducialSegmentation.Line3 = Line3ROI;
        FiducialSegmentation.Line3(3) = FiducialSegmentation.Line3(3) - FiducialSegmentation.Line3(1) * Xoffset - FiducialSegmentation.Line3(2) * Yoffset;
    else
        FiducialSegmentation.Line3 = [];
    end
    
    if ~isempty(Ellipse1ROI)
        FiducialSegmentation.Ellipse1 = EllipseTranslate(Ellipse1ROI, -Xoffset, -Yoffset);
    else
        FiducialSegmentation.Ellipse1 = [];
    end

    if ~isempty(Ellipse2ROI)
        FiducialSegmentation.Ellipse2 = EllipseTranslate(Ellipse2ROI, -Xoffset, -Yoffset);
    else
        FiducialSegmentation.Ellipse2 = [];
    end
    

    % display results
    axes(handles.ImageDisplay);
    DisplayFiducialSegmentation();
    UpdateStatus(hObject, handles);

    set(gcf, 'pointer', 'arrow');    
    set(handles.UserAlertMsg, 'string', '');
    drawnow;


    
    
    
% --- Executes on mouse press over figure background, over a disabled or
% --- inactive control, or over an axes background.
function FiducialSegmentationGUI_WindowButtonDownFcn(hObject, eventdata, handles)
% hObject    handle to FiducialSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FiducialSegmentation;
    
    CurrentAxesHandle = handles.ImageDisplay;
    CurrentPoint = [];
    CurrentPoint = get(CurrentAxesHandle, 'currentpoint');

    switch(FiducialSegmentation.TaskIdentifier)
        case 1

        case 2

        case 3
            % add BB on Fiducial
            set(gcf, 'pointer', 'watch');
            XToAdd = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
            YToAdd = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;

            InsertIndex = find(FiducialSegmentation.BBs(:,1) == -1);
            if (InsertIndex)
                PointToAdd = LocateDarkestPoint(FiducialSegmentation.I, [XToAdd YToAdd], 3);
                FiducialSegmentation.BBs(InsertIndex(1),:) = PointToAdd;
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            else
                set(gcf, 'pointer', 'crosshair');
                msgbox('There cannot be more than 9 BBs; please remove at least one BB to add one', 'Error adding', 'warn', 'modal');                
            end
            set(gcf, 'pointer', 'crosshair');
        
        case 4
            % delete BB on Fiducial  
            set(gcf, 'pointer', 'watch');
            XToDelete = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
            YToDelete = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;

            iBBIndex = FindSeed(FiducialSegmentation.BBs, [XToDelete YToDelete]);

            if (iBBIndex > 0)
                DelBB = FiducialSegmentation.BBs(iBBIndex, 1:2);
                FiducialSegmentation.BBs(iBBIndex, :) = [-1 -1];
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            else
                set(gcf, 'pointer', 'crosshair');
                msgbox('No BB in close vicinity found to delete, try again by clicking closer to the BB you wish to delete', 'Error deleting seed', 'warn', 'modal');                           
            end
            set(gcf, 'pointer', 'crosshair');
        
        case 5
            % add line  
            if (~isempty(FiducialSegmentation.Line1) && ~isempty(FiducialSegmentation.Line2) && ~isempty(FiducialSegmentation.Line3))
                msgbox('There cannot be more than 3 lines; please remove at least one line to add one', 'Error adding', 'warn', 'modal');    
            else
                XToAdd = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
                YToAdd = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;
                FiducialSegmentation.UserPoints = [FiducialSegmentation.UserPoints; XToAdd YToAdd];
            
                if (size(FiducialSegmentation.UserPoints, 1) == FiducialSegmentation.PointsReqLine)
                    set(gcf, 'pointer', 'watch');
                    lineEq = least_squares_line(FiducialSegmentation.UserPoints);                    
                    if isempty(FiducialSegmentation.Line1)
                        FiducialSegmentation.Line1 = lineEq;                        
                    else 
                        if isempty(FiducialSegmentation.Line2)
                            FiducialSegmentation.Line2 = lineEq;                        
                        else
                            FiducialSegmentation.Line3 = lineEq;                        
                        end
                    end
                    FiducialSegmentation.UserPoints = [];         
                    set(gcf, 'pointer', 'crosshair');
                end
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            end
        
       

        case 6
            % delete a line
            set(gcf, 'pointer', 'watch');
            FoundLine = false;
            X = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
            Y = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;
            
            % find the line on which the point lies
            % line 1
            if ~isempty(FiducialSegmentation.Line1)
                Distance = FiducialSegmentation.Line1(1)*X + FiducialSegmentation.Line1(2)*Y + FiducialSegmentation.Line1(3);
                if (abs(Distance) <= 3)
                    FiducialSegmentation.Line1 = [];
                    FoundLine = true;
                end    
            end

            if ~isempty(FiducialSegmentation.Line2)
                Distance = FiducialSegmentation.Line2(1)*X + FiducialSegmentation.Line2(2)*Y + FiducialSegmentation.Line2(3);
                if (abs(Distance) <= 3)
                    FiducialSegmentation.Line2 = [];
                    FoundLine = true;
                end    
            end

            if ~isempty(FiducialSegmentation.Line3)
                Distance = FiducialSegmentation.Line3(1)*X + FiducialSegmentation.Line3(2)*Y + FiducialSegmentation.Line3(3);
                if (abs(Distance) <= 3)
                    FiducialSegmentation.Line3 = [];
                    FoundLine = true;
                end    
            end
                   
            if (~FoundLine)
                set(gcf, 'pointer', 'crosshair');
                msgbox('No Line in close vicinity found to delete, try again by clicking closer to the line you wish to delete', 'Error removing line', 'warn', 'modal');
            else
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            end
            set(gcf, 'pointer', 'crosshair');

            
        case 7
            % add ellipse
            if (~isempty(FiducialSegmentation.Ellipse1) && ~isempty(FiducialSegmentation.Ellipse2))
                msgbox('There cannot be more than 2 ellipse; please remove at least one ellipse to add one', 'Error adding', 'warn', 'modal');    
            else
                XToAdd = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
                YToAdd = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;
                FiducialSegmentation.UserPoints = [FiducialSegmentation.UserPoints; XToAdd YToAdd];
            
                if (size(FiducialSegmentation.UserPoints, 1) == FiducialSegmentation.PointsReqEllipse)
                    set(gcf, 'pointer', 'watch');
                    ellipseEq = EllipseEqFromPoints(FiducialSegmentation.UserPoints(:,1), FiducialSegmentation.UserPoints(:,2));
                    
                    if isempty(FiducialSegmentation.Ellipse1)
                        FiducialSegmentation.Ellipse1 = ellipseEq;
                    else
                        FiducialSegmentation.Ellipse2 = ellipseEq;
                    end
                    FiducialSegmentation.UserPoints = [];         
                    set(gcf, 'pointer', 'crosshair');
                end
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            end
            
       
        case 8
            % delete an ellipse
            set(gcf, 'pointer', 'crosshair');
            X = round(CurrentPoint(1,2)) + FiducialSegmentation.ROI.StartX - 1;
            Y = round(CurrentPoint(1,1)) + FiducialSegmentation.ROI.StartY - 1;
       
            % see if the point belongs to the ellipse?
            if ~isempty(FiducialSegmentation.Ellipse1)
                eq = FiducialSegmentation.Ellipse1;
                Distance1 = abs(eq(1)*X*X + eq(2)*X*Y + eq(3)*Y*Y + eq(4)*X + eq(5)*Y + eq(6));
            else
                Distance1 = 100;
            end
            
            if ~isempty(FiducialSegmentation.Ellipse2)
                eq = FiducialSegmentation.Ellipse2;
                Distance2 = abs(eq(1)*X*X + eq(2)*X*Y + eq(3)*Y*Y + eq(4)*X + eq(5)*Y + eq(6));
            else
                Distance2 = 100;
            end

            MinDistance = min([Distance1 Distance2]);
            if (MinDistance <=1)
                if (Distance1 == MinDistance)
                    % remove ellipse 1
                    FiducialSegmentation.Ellipse1 = [];
                else
                    FiducialSegmentation.Ellipse2 = [];
                end
                set(gcf, 'pointer', 'crosshair');
                DisplayFiducialSegmentation();
                UpdateStatus(hObject, handles);
            else
                set(gcf, 'pointer', 'crosshair');
                msgbox('No ellipse in close vicinity found to delete, try again by clicking closer to the ellipse you wish to delete', 'Error removing line', 'warn', 'modal');           
            end
      
        otherwise
            CurrentPoint
    end


    

% $Log: FiducialSegmentationGUI.m,v $
% Revision 1.10  2007/09/19 14:50:17  anton
% Segmentation: Take Ratio to NTSC into account for ellipses
%
% Revision 1.9  2007/08/03 22:37:16  anton
% FiducialSegmentationGUI.m:  Added "delete all" button, separated BBs from
% lines and ellipses segmentation.  Now working at diff. resolutions for all but
% ellipses.
%
% Revision 1.8  2007/08/01 19:08:59  anton
% FiducialSegmentationGUI.m: Added support for resRatioToNTSC.  Added code
% to change cursor to watch when computing.
%
% Revision 1.7  2007/04/02 20:05:10  anton
% Fiducial segmentation: Added display of user input, existing points entered
% for lines and ellipses
%
% Revision 1.6  2007/01/17 04:03:24  anton
% FiducialSegmentationGUI.m: Added Swap Ellipses feature
%
% Revision 1.5  2007/01/10 17:21:59  anton
% FiducialSegmentationGUI.m: Working version, removed dead code, added
% status to display nb BBs, lines, fiducial, user defined points.
%
% Revision 1.4  2007/01/10 05:17:03  anton
% FiducialSegmentationGUI.m:  Added callback on image and updated code
% for adding/deleting BBs, removing lines.  Still missing code for ellipses and
% add line.
%
% Revision 1.3  2007/01/10 04:14:22  anton
% FiducialSegmentationGUI: Version with working automatic segmentation.
% Still needs the manual edit features.
%
% Revision 1.2  2007/01/08 19:57:43  anton
% Segmentation: New code for fiducial segmentation.  Working version for
% lines, ellipses still not working.
%
% Revision 1.1  2007/01/03 22:25:05  anton
% Segmentation: Preliminary code for fiducial segmentation in separate window.  NOT WORKING YET.
%
% Revision 1.16  2006/09/22 21:55:40  anton
% Segmentation: Added option to look for closest point or not when adding
% seeds by hand
%
% Revision 1.15  2006/04/21 14:48:36  anton
% Segmentation GUI: Removed use of rufMainGUIData in SeedAndSegmentation
% GUI to keep the segmentation usable as stand-alone.
%
% Revision 1.14  2006/04/20 20:43:16  anton
% Segmentation: SeedandFiducialSegementationGUI, major update:
% *: Added buttons "abort" and "accept both"
% *: Change logic so that one can segment in any order (seeds/fiducial)
% *: Factor some code to control button state (enable/disable)
% *: Added flags to check that both seeds and fiducial are segmented
%
% Revision 1.13  2006/04/18 14:58:38  anton
% Segmentation: Added "Abort" button.  Work in progress.
%
% Revision 1.12  2005/12/28 02:46:30  jain
% Some more error checking before returning from the GUI. Added a global flag to
% check if the GUI was killed or or not. Returning this flag and the handle too.
% Clearning global variables after returning.
%
% Revision 1.11  2005/12/23 00:51:17  jain
% The five files from Siddharth that I got are added here. Also I checked if these files
% are from which version - looks like cvs does not change the dates of the files and
% hence the confusion. This version is stable with bad cases, but output is still to be
% taken care of - uiwait will be added in the next version.
%
% Revision 1.8  2005/11/28 19:45:14  anton
% General Update from Siddharth:
% *: Handling failure cases in fiducial segmentation, when only partial region
%    is selected as region of interest for fiducial...however, note that the
%    region should not be too small
% *: Now the BB numbers, line numbers and ellipse numbers are displayed after
%    establishing correspondence to have a visual check for user that
%    correspondence established is correct; Note tht these are displayed only
%    on whole image, not zoomed up region of interest, because there user can
%    make changes....so just on whole image after segmentation & after user
%    says he accepts the results
% *: Few cases prev. unhandled in user intervention in fiducial segmentation
%    validation
%
% Revision 1.7  2005/11/08 00:47:51  svikal
% BB correspondence after edit/user changes
%
% Revision 1.6  2005/11/07 00:05:11  svikal
% integrating with FTRAC;output of segmentation in desired format of FTRAC
%
% Revision 1.5  2005/11/05 22:56:12  svikal
% improved ellipse detection; changed the name of the function of fitting ellipse; hence file changed; also comments
%
% Revision 1.4  2005/11/03 15:54:16  svikal
% changes made for establishing correspondence of BBs
%
% Revision 1.3  2005/10/31 19:39:29  svikal
% few comments and ellipse final equ.but still commented
%
% Revision 1.2  2005/10/27 18:28:51  svikal
% Added CVS tags
%


