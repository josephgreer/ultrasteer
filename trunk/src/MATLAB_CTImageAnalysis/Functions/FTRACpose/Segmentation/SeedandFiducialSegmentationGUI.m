% $Id: SeedandFiducialSegmentationGUI.m,v 1.16 2006/09/22 21:55:40 anton Exp $

function varargout = SeedandFiducialSegmentationGUI(varargin)
% SEEDANDFIDUCIALSEGMENTATIONGUI M-file for SeedandFiducialSegmentationGUI.fig
%      SEEDANDFIDUCIALSEGMENTATIONGUI, by itself, creates a new SEEDANDFIDUCIALSEGMENTATIONGUI or raises the existing
%      singleton*.
%
%      H = SEEDANDFIDUCIALSEGMENTATIONGUI returns the handle to a new SEEDANDFIDUCIALSEGMENTATIONGUI or the handle to
%      the existing singleton*.
%
%      SEEDANDFIDUCIALSEGMENTATIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SEEDANDFIDUCIALSEGMENTATIONGUI.M with the given input arguments.
%
%      SEEDANDFIDUCIALSEGMENTATIONGUI('Property','Value',...) creates a new SEEDANDFIDUCIALSEGMENTATIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before SeedandFiducialSegmentationGUI_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to SeedandFiducialSegmentationGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SeedandFiducialSegmentationGUI

% Last Modified by GUIDE v2.5 22-Sep-2006 17:04:01

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SeedandFiducialSegmentationGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @SeedandFiducialSegmentationGUI_OutputFcn, ...
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


% --- Executes just before SeedandFiducialSegmentationGUI is made visible.
function SeedandFiducialSegmentationGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to SeedandFiducialSegmentationGUI (see VARARGIN)

%Clearing the global variables used for output. This way values from the
%previous run will not be carried over. Added by Ameet.
clear global SeedSegmentationFinalInformationDataSet;
clear global BBLocations;
clear global lLine1PointsEq;
clear global lLine2PointsEq;
clear global lLine3PointsEq;
clear global lEllipse1Points;
clear global lEllipse2Points;
clear global Segmentation_Complete_Flag;
global Segmentation_Complete_Flag;
Segmentation_Complete_Flag = false;

clear global rufSegmentationData;
global rufSegmentationData;
rufSegmentationData.SeedSegmentationPerformed = false;
rufSegmentationData.FiducialSegmentationPerformed = false;
DoneButtonEnable(handles);

% Choose default command line output for SeedandFiducialSegmentationGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SeedandFiducialSegmentationGUI wait for user response (see UIRESUME)
% uiwait(handles.SeedandFiducialSegmentationGUI);

% varargin: we pass opened image buffer as input to this GUI based function; 

global I;

% open the file, disply it
[I] = varargin{1};

CurrentImageHandle = imshow(I); 

%  for enabling select fiducial region, to allow for fiducial segmentation
%  b4 seed segmentation; only for debug purposes, not the normal flow of
%  program
% set(handles.FiducialSelectROIButton, 'enable', 'on');

movegui('center');



function DoneButtonEnable(handles)
    global rufSegmentationData;
    if (rufSegmentationData.SeedSegmentationPerformed && rufSegmentationData.FiducialSegmentationPerformed)
        set(handles.DoneButton, 'enable', 'on');
    else
        set(handles.DoneButton, 'enable', 'off');
    end



function SeedSegmentationControlEnable(handles, onOff)
    set(handles.AddSeedToggleButton, 'enable', onOff);
    set(handles.DeleteSeedToggleButton, 'enable', onOff);
    set(handles.UndoSeedChangeButton, 'enable', onOff);
    set(handles.DeleteAllSeedsButton, 'enable', onOff);
    set(handles.SeedSegmentationAcceptButton, 'enable', onOff);




function FiducialSegmentationControlEnable(handles, onOff)
    set(handles.FiducialSegmentationAcceptButton, 'enable', onOff);
    set(handles.AddBBToggleButton, 'enable', onOff);
    set(handles.AddLineToggleButton, 'enable', onOff);
    set(handles.AddEllipseToggleButton, 'enable', onOff);
    set(handles.DeleteBBToggleButton, 'enable', onOff);
    set(handles.DeleteLineToggleButton, 'enable', onOff);
    set(handles.DeleteEllipseToggleButton, 'enable', onOff);
    set(handles.UndoFiducialChangeButton, 'enable', onOff);




function FiducialSegmentationControlToggle(handles, hObject, newTaskIdentifier) 
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');

    if (button_state == get(hObject,'Max'))
    
        if (iGlobalTaskIdentifier == 3)
            set(handles.AddBBToggleButton, 'Value', 0);
            set(handles.AddBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end
    
        if (iGlobalTaskIdentifier == 4)
            set(handles.DeleteBBToggleButton, 'Value', 0);
            set(handles.DeleteBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end
    
        if (iGlobalTaskIdentifier == 5)
            set(handles.AddLineToggleButton, 'Value', 0);
            set(handles.AddLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end

        if (iGlobalTaskIdentifier == 6)
            set(handles.DeleteLineToggleButton, 'Value', 0);
            set(handles.DeleteLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end

        if (iGlobalTaskIdentifier == 7)
            set(handles.AddEllipseToggleButton, 'Value', 0);
            set(handles.AddEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end

        if (iGlobalTaskIdentifier == 8)
            set(handles.DeleteEllipseToggleButton, 'Value', 0);
            set(handles.DeleteEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
        end

        iGlobalTaskIdentifier = newTaskIdentifier;
        set(gcf, 'pointer', 'crosshair');
    
    elseif (button_state == get(hObject,'Min'))
        % reset the add pursuit
        iGlobalTaskIdentifier = 0;
        set(handles.UserAlertMsg, 'string', '');
        set(gcf, 'pointer', 'arrow');
    end

    
    

% --- Outputs from this function are returned to the command line.
function varargout = SeedandFiducialSegmentationGUI_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure

%Uiwait and changed output inserted by by Ameet
global SeedSegmentationFinalInformationDataSet;
global BBLocations;
global lLine1PointsEq;
global lLine2PointsEq;
global lLine3PointsEq;
global lEllipse1Points; 
global lEllipse2Points;
global Segmentation_Complete_Flag;

uiwait(handles.output)

varargout{1} = handles.output;
varargout{2} = Segmentation_Complete_Flag;
if (Segmentation_Complete_Flag == false)
    for tmp_counter = 3 : 9
        varargout{tmp_counter} = [];
    end
else   
    varargout{3} = SeedSegmentationFinalInformationDataSet;
    varargout{4} = BBLocations;
    varargout{5} = lLine1PointsEq;
    varargout{6} = lLine2PointsEq;
    varargout{7} = lLine3PointsEq;
    if(isempty(lEllipse1Points))
        ellipse_1_eq = [];
    else
        [ellipse1 ellipse_1_eq] = best_fit_ellipse(lEllipse1Points(:,1), lEllipse1Points(:,2));
    end
    varargout{8} = ellipse_1_eq;
    if(isempty(lEllipse2Points))
        ellipse_2_eq = [];
    else
        [ellipse2 ellipse_2_eq] = best_fit_ellipse(lEllipse2Points(:,1), lEllipse2Points(:,2));
    end
    varargout{9} = ellipse_2_eq;
end

clear global SeedSegmentationFinalInformationDataSet;
clear global BBLocations;
clear global lLine1PointsEq;
clear global lLine2PointsEq;
clear global lLine3PointsEq;
clear global lEllipse1Points;
clear global lEllipse2Points;
clear global Segmentation_Complete_Flag;






% //-----------------------------------------------------------------------------
% // function:	SeedSelectROIButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Select region button is clicked
%                   on the GUI, allows the user to select region of
%                   interest where the implanted seeds are, this function
%                   further calls seed segmentation routine to perform the
%                   segmentation, display routine to display the results,
%                   after completeion of segmentation also activates
%                   further buttons to enable the user to accept the
%                   segmentation results or make changes
% // input:	global variable I (image buffer)
% // output: global variables: 
% variables that identify the region of interest coordinates
%           global seedROIStartX; 
%           global seedROIStartY; 
%           global seedROIXWidth; 
%           global seedROIYWidth;
%           global SeedSegmentationROITranslationVector;
% variables in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current image handle
%             global CurrentImageHandle;
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global SeedUndoTaskList;
% // calling function: called when Select region button is pressed
% // NOTE: Now, the BrachytherapySeedSegmentation can be called
% either by using providing external threshold value for
% thersholding operation performed after morphological top hat opening
% operation to be 20 or let the function internally calculate a threshold value
% on its own automatically; the value 20 has been arrived at by experience of
% images; 
% //-----------------------------------------------------------------------------

% --- Executes on button press in SeedSelectROIButton.
function SeedSelectROIButton_Callback(hObject, eventdata, handles)
% hObject    handle to SeedSelectROIButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% global variable required:
global  I;

% seed segmentation related global variables
global seedROIStartX; global seedROIStartY; global seedROIXWidth; global seedROIYWidth;
global SeedSegmentationFinalInformationDataSet;
global SeedSegmentationROITranslationVector;
global FloatingSeedSegmentationFinalInformationDataSet;
global CurrentImageHandle;
global iGlobalTaskIdentifier; 
global SeedUndoTaskList;


% get  a rectangular region of interest
rect = getrect(handles.ImageDisplay);
% get the corners of the rectangle
seedROIStartY = round(rect(1,1)); seedROIStartX = round(rect(1,2)); seedROIYWidth = round(rect(1,3)); seedROIXWidth = round(rect(1,4));

% effect changes in states of various button or controls to reflect the
% current state
set(handles.SeedSelectROIButton, 'enable', 'off');
set(handles.FiducialSelectROIButton, 'enable', 'off');
set(handles.DoneButton, 'enable', 'off');
set(handles.UserAlertMsg, 'string', 'Performing seed segmentation.......');
set(gcf, 'pointer', 'watch');

% perform segementation
% final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]

[SeedSegmentationFinalInformationDataSet iThresholdInUse]= BrachytherapySeedSegmentation(I(seedROIStartX:seedROIStartX + seedROIXWidth, seedROIStartY:seedROIStartY + seedROIYWidth), true,20);

% copy results to a buffer which will be modified by user inputs/changes
FloatingSeedSegmentationFinalInformationDataSet = SeedSegmentationFinalInformationDataSet;

% identifies, what task user is et to perform
iGlobalTaskIdentifier = 0;

% will maintain the list of actions taken by user in sequence, so that the
% task can be undone if desired
SeedUndoTaskList = [];


% compile the results & display them
% traslation vector required, since the current results are in the
% coordinate system of the Region of interest, -1 because image array is
% indexed from 1, 1
SeedSegmentationROITranslationVector = [(seedROIStartX-1) (seedROIStartY-1)];

imshow(I);

% display the results on whole image
DisplaySeedSegmentationWholeImage;

pause(0.5);

% display results only on region of interest, gives the zoom effect
DisplaySeedSegmentationROI;

% user message
set(gcf, 'pointer', 'arrow');
set(handles.UserAlertMsg, 'string', 'Seed segmentation complete, press "Accept" or "Add/Delete seed" to proceed further......');
set(handles.Threshold, 'enable', 'on');
strThresh = sprintf('Theshold = %d', round(iThresholdInUse));
set(handles.ThresholdDisplay, 'string', strThresh);
set(handles.Threshold, 'value', iThresholdInUse);

SeedSegmentationControlEnable(handles, 'on');








% //-----------------------------------------------------------------------------
% // function:	SeedSegmentationAcceptButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Accept button is clicked
%                   on the GUI, tasks done:
%                   1) disable all other button related to seed
%                   segmentation procedure,
%                   2) copy the floating buffer of segmentation results to
%                   the original/final results buffer
%                   3) enable the 'Select region' button of Fiducial
%                   Segmentation to enable the user to proceed further
% // input:	global variable I (image buffer)
% variables in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% //output:  
% variable in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% // calling function: called when Select region button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in SeedSegmentationAcceptButton.
function SeedSegmentationAcceptButton_Callback(hObject, eventdata, handles)
% hObject    handle to SeedSegmentationAcceptButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% global variable required:
% seed segmentation related global variables
global SeedSegmentationFinalInformationDataSet;
global SeedSegmentationROITranslationVector;
global FloatingSeedSegmentationFinalInformationDataSet;
global iGlobalTaskIdentifier; 
global I;

global rufSegmentationData;
rufSegmentationData.SeedSegmentationPerformed = true;
DoneButtonEnable(handles);

% manage the states of the toggle buttons, to give 'visual effect' 

if (iGlobalTaskIdentifier == 1)
    bAddSeedPursuitOn = false;
    set(handles.AddSeedToggleButton, 'Value', 0);
    set(handles.AddSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end


if (iGlobalTaskIdentifier == 2)
    bDeleteSeedPursuitOn = false;
    set(handles.DeleteSeedToggleButton, 'Value', 0);
    set(handles.DeleteSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end
    
iGlobalTaskIdentifier = 0;

set(gcf, 'pointer', 'arrow');
    
% to do 
% transform the results from Region Of Interest coordinate system to actual Image coordinate system 
SeedSegmentationFinalInformationDataSet = FloatingSeedSegmentationFinalInformationDataSet;

imshow(I);

% display final results overlaid on original image
DisplaySeedSegmentationWholeImage;
DisplayFiducialSegmentationWholeImage;

set(handles.Threshold, 'enable', 'off');
SeedSegmentationControlEnable(handles, 'off');
set(handles.SeedSelectROIButton, 'enable', 'on');
set(handles.FiducialSelectROIButton, 'enable', 'on');
set(handles.UserAlertMsg, 'string', 'Seed segmentation results accepted.');















% //-----------------------------------------------------------------------------
% // function:	AddSeedToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Add seed toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTask Identifier
%                   to 1 incdicating the current task is to add seeds
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
% // calling function: called when AddSeed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in AddSeedToggleButton.
function AddSeedToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddSeedToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddSeedToggleButton

% global variable required:
global iGlobalTaskIdentifier; 



% manage the state/color of the add button
button_state = get(hObject,'Value');

if( button_state == get(hObject,'Max'))
    % toggle button is pressed
    
    % change the foreground color
    set(handles.AddSeedToggleButton, 'BackgroundColor', 'white');


    if (iGlobalTaskIdentifier == 2)
        bDeleteSeedPursuitOn = false;
        set(handles.DeleteSeedToggleButton, 'Value', 0);
        set(handles.DeleteSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end
    
    % set the add pursuit
    iGlobalTaskIdentifier = 1;

    set(handles.UserAlertMsg, 'string', 'Add seed active: Click on image to add a seed');
    
    set(gcf, 'pointer', 'crosshair');
    
elseif( button_state == get(hObject,'Min'))
    % toggle button is not pressed
    % change the foreground color
    set(handles.AddSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);

    % reset the add pursuit
    iGlobalTaskIdentifier = 0;
    
    set(handles.UserAlertMsg, 'string', '');
    
    set(gcf, 'pointer', 'arrow');
    
end
% manage the state/color of the delete button
% give a user alert message of what he/she is goin to do

% start taking input in a while loop until delete or accept button is
% pressed or this task is interrupted in some manner










% //-----------------------------------------------------------------------------
% // function:	DeleteSeedToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Delete seed toggle button is clicked
%                   on the GUI; first determines the current state of the
%                   button & toggles it, if the resulting state is 'on',
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) then sets the value of variable iGlobalTaskIdentifier
%                   to 2 incdicating the current task is to delete the
%                   nearest seed wherever the user clicks until the state of the button
%                   is changed
% // input:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // output: 
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% // calling function: called when Delete seed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in DeleteSeedToggleButton.
function DeleteSeedToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteSeedToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of DeleteSeedToggleButton
% global variable required:

% seed segmentation related global variables
global iGlobalTaskIdentifier; 



% manage the state/color of the add button
button_state = get(hObject,'Value');

if( button_state == get(hObject,'Max'))
    % toggle button is pressed
    
    % change the foreground color
    set(handles.DeleteSeedToggleButton, 'BackgroundColor', 'white');


    if (iGlobalTaskIdentifier == 1)
        bAddSeedPursuitOn = false;
        set(handles.AddSeedToggleButton, 'Value', 0);
        set(handles.AddSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end
    

    iGlobalTaskIdentifier = 2;
    
    set(handles.UserAlertMsg, 'string', 'Delete seed active: Click on seed to remove');
    
    set(gcf, 'pointer', 'crosshair');
    
elseif( button_state == get(hObject,'Min'))
    % toggle button is not pressed
    % change the foreground color
    set(handles.DeleteSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);


    iGlobalTaskIdentifier = 0;
    
    set(handles.UserAlertMsg, 'string', '');
    
    set(gcf, 'pointer', 'arrow');
end
% manage the state/color of the delete button
% give a user alert message of what he/she is goin to do

% start taking input in a while loop until delete or accept button is
% pressed or this task is interrupted in some manner












% //-----------------------------------------------------------------------------
% // function:	UndoSeedChangeButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Undo button (Seed segementation group) 
%                   is clicked on the GUI; 
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) Retrieves the last action performed by the user
%                   from the SeedUndoTaskList, if the last action was
%                   AddSeed then it deletes the corresponding seed; if the
%                   last seed was DeleteSeed then it adds the corresponding
%                   seed
% // input:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global SeedUndoTaskList;
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global SeedUndoTaskList;
% // calling function: called when Delete seed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in UndoSeedChangeButton.
function UndoSeedChangeButton_Callback(hObject, eventdata, handles)
% hObject    handle to UndoSeedChangeButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


global FloatingSeedSegmentationFinalInformationDataSet;
global SeedUndoTaskList;
global iGlobalTaskIdentifier;
% SeedUndoTaskList = [TaskId X Y]; note X, Y are in display coordinate
% system, not our coordinate system


% manage the state of toggle buttons

if (iGlobalTaskIdentifier == 1)
    bAddSeedPursuitOn = false;
    set(handles.AddSeedToggleButton, 'Value', 0);
    set(handles.AddSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end


if (iGlobalTaskIdentifier == 2)
    bDeleteSeedPursuitOn = false;
    set(handles.DeleteSeedToggleButton, 'Value', 0);
    set(handles.DeleteSeedToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

iGlobalTaskIdentifier = 0;

set(gcf, 'pointer', 'arrow');

% Logic: pop up the first task, identify the nature of the task
if (length(SeedUndoTaskList) > 1)
    set(gcf, 'pointer', 'watch');    
    set (hObject, 'enable', 'off');
    iTaskIdentifier = SeedUndoTaskList(1,1);
    
    switch(iTaskIdentifier)
        case (2), 
    %       initially deleted, so undo delete i.e. add a seed
            XToAdd = SeedUndoTaskList(1,2);
            YToAdd = SeedUndoTaskList(1,3);

            % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
            FloatingSeedSegmentationFinalInformationDataSet = [FloatingSeedSegmentationFinalInformationDataSet; YToAdd XToAdd 0 0 0];   
            hold on;
            plot(XToAdd, YToAdd, 'g.', 'ButtonDownFcn', 'ImageAxesClick_Callback');
            hold off;
            set(handles.UserAlertMsg, 'string', 'Seed initially deleted was added back');

        case (1), 
    %       intially added, so undo add i.e. delete/remove a seed  
            XToDelete = SeedUndoTaskList(1,2);
            YToDelete = SeedUndoTaskList(1,3);
            % final data set: [CGx CGy Orientation OrientationValidity PartOfCluster]
            ToFindCG = [];
            ToFindCG = [YToDelete XToDelete];
            iSeedIndex = FindSeed(FloatingSeedSegmentationFinalInformationDataSet, ToFindCG);
            FloatingSeedSegmentationFinalInformationDataSet (iSeedIndex, :) = [];    
            DisplaySeedSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'Seed initially added was removed');
            
    end
    
    SeedUndoTaskList(1,:) = [];
    set (hObject, 'enable', 'on');
    set(gcf, 'pointer', 'arrow');
else
    set(handles.UserAlertMsg, 'string', 'No action to undo');
end    









% --- Executes during object creation, after setting all properties.
function Threshold_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Threshold (see GCBO)
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








% //-----------------------------------------------------------------------------
% // function:	Threshold_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Threshold slider is moved 
%                   on the GUI, performs the seed segmentation again with
%                   new value of threshold; allows for user threshold input
% // input:	global variable I (image buffer)
% // output: global variables: 
% variables that identify the region of interest coordinates
%           global seedROIStartX; 
%           global seedROIStartY; 
%           global seedROIXWidth; 
%           global seedROIYWidth;
%           global SeedSegmentationROITranslationVector;
% variables in which the results will be put
%             global SeedSegmentationFinalInformationDataSet;
% variable which is a floating buffer for the results, which is modified
% when user performs Add or Delete seeds, all the changes are made to this
% buffer rather than original, it is copied onto original when user clicks
% Accept button
%             global FloatingSeedSegmentationFinalInformationDataSet;
% variable that identifies the current image handle
%             global CurrentImageHandle;
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global SeedUndoTaskList;
% // calling function: called when Select region button is pressed
% // NOTE: This function allows for user threshold input 
% //-----------------------------------------------------------------------------
% --- Executes on slider movement.
function Threshold_Callback(hObject, eventdata, handles)
% hObject    handle to Threshold (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider

% global variable required:
global  I;

% seed segmentation related global variables
global seedROIStartX; global seedROIStartY; global seedROIXWidth; global seedROIYWidth;
global SeedSegmentationFinalInformationDataSet;
global SeedSegmentationROITranslationVector;
global FloatingSeedSegmentationFinalInformationDataSet;
global CurrentImageHandle;
global iGlobalTaskIdentifier; 
global SeedUndoTaskList;


strThresh = sprintf('Theshold = %d', round(get(handles.Threshold, 'Value')));
set(handles.ThresholdDisplay, 'string', strThresh);

SeedSegmentationControlEnable(handles, 'off');
set(handles.UserAlertMsg, 'string', 'Performing seed segmentation.......');
set(gcf, 'pointer', 'watch');
% perform segementation
[SeedSegmentationFinalInformationDataSet iThresholdInUse] = BrachytherapySeedSegmentation(I(seedROIStartX:seedROIStartX + seedROIXWidth, seedROIStartY:seedROIStartY + seedROIYWidth), true, round(get(handles.Threshold, 'Value')));

FloatingSeedSegmentationFinalInformationDataSet = SeedSegmentationFinalInformationDataSet;
% compile the results 
% traslation vector required, since the current results are in the
% coordinate system of the Region of interest, -1 because image array is
% indexed from 1, 1
SeedSegmentationROITranslationVector = [(seedROIStartX-1) (seedROIStartY-1)];

iGlobalTaskIdentifier = 0;

SeedUndoTaskList = [];

imshow(I);

DisplaySeedSegmentationWholeImage;
DisplayFiducialSegmentationWholeImage;

pause(0.5);

DisplaySeedSegmentationROI;

set(CurrentImageHandle, 'ButtonDownFcn', 'ImageAxesClick_Callback');

% manage the states of the buttons
set(gcf, 'pointer', 'arrow');
set(handles.UserAlertMsg, 'string', 'Seed segmentation complete, press "Accept" or "Add/Delete seed" to proceed further......');
SeedSegmentationControlEnable(handles, 'on');










% //-----------------------------------------------------------------------------
% // function:	FiducialSelectROIButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
% --- Executes on button press in FiducialSelectROIButton.
function FiducialSelectROIButton_Callback(hObject, eventdata, handles)
% hObject    handle to FiducialSelectROIButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

global I; 
global FiducialROIStartX; global FiducialROIStartY; global FiducialROIXWidth; global FiducialROIYWidth;
global FiducialSegmentationROITranslationVector;
global BBLocations;
global lLine1PointsEq;global lLine2PointsEq;global lLine3PointsEq; 
global lEllipse1Points;global lEllipse2Points; 

global Floating_BBLocations;

global Floating_lLine1PointsEq;global Floating_lLine2PointsEq;global Floating_lLine3PointsEq; 

global IROIFiducial;
global Line1StartingPoint;global Line2StartingPoint;global Line3StartingPoint;
global bLine1Removed; global bLine2Removed; global bLine3Removed;
bLine1Found = false;
bLine2Found = false;
bLine3Found = false;


global NumOfPointsToDefineLine; global PointsOnLine;

global NumOfPointsToDefineEllipse; global PointsOnEllipse;
global Floating_lEllipse1Points;global Floating_lEllipse2Points;
global bEllipse1Removed; global bEllipse2Removed;

global iGlobalTaskIdentifier; 
global FiducialUndoTaskList;

% get  a rectangular region of interest
rect = getrect(handles.ImageDisplay);
FiducialROIStartY = round(rect(1,1)); FiducialROIStartX = round(rect(1,2)); FiducialROIYWidth = round(rect(1,3)); FiducialROIXWidth = round(rect(1,4));

set(handles.SeedSelectROIButton, 'enable', 'off');
set(handles.FiducialSelectROIButton, 'enable', 'off');
set(handles.DoneButton, 'enable', 'off');
set(handles.UserAlertMsg, 'string', 'Performing fiducial segmentation.......');
set(gcf, 'pointer', 'watch');

% Step 1; first perform the fiducial BB segmentation; 
%          a) get the BB locations
FiducialSegmentationROITranslationVector = [(FiducialROIStartX - 1) (FiducialROIStartY - 1)];

BBLocations = FiducialBBSegmentation (I(FiducialROIStartX:FiducialROIStartX + FiducialROIXWidth, FiducialROIStartY:FiducialROIStartY + FiducialROIYWidth) );

set(handles.UserAlertMsg, 'string', 'Segmenting Lines.....');
% Step 2: form an edge image
IROIFiducial = double(I(FiducialROIStartX:FiducialROIStartX + FiducialROIXWidth, FiducialROIStartY:FiducialROIStartY + FiducialROIYWidth));

InegROIFiducial = double(imcomplement(I(FiducialROIStartX:FiducialROIStartX + FiducialROIXWidth, FiducialROIStartY:FiducialROIStartY + FiducialROIYWidth)));

FiducialEdgeImage = [];

FiducialEdgeImage = BuildEdgeImage(InegROIFiducial,2);

iX_Dimension = length (FiducialEdgeImage(:,1));

iY_Dimension = length (FiducialEdgeImage(1,:));



% Step 3: perform line detection by graph search

% first locate line's estimated start and end points from corresponding BBs located
% locate the estimated point from the earlier found BB locations
[Line1StartingPoint, Line1EndPoint, Line2StartingPoint, Line2EndPoint, Line3StartingPoint, Line3EndPoint] = GetLineStartAndEndPointsFromBBs(BBLocations, length(FiducialEdgeImage(:,1)));

% find direction estimate if both start and end points are available for
% any line
[bUseDirectionEstimate InitialDirectionEstimate] = CalculateLineDirectionEstimateFromEndPoints(Line1StartingPoint, Line1EndPoint, Line2StartingPoint, Line2EndPoint, Line3StartingPoint, Line3EndPoint, iX_Dimension, iY_Dimension);

% each line one by one
% 1st line

if (length(Line1StartingPoint) > 1 || length(Line1EndPoint) > 1)
    [bLine1Found lLine1PointsEq bStartPointValid bEndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,Line1StartingPoint, Line1EndPoint, bUseDirectionEstimate, InitialDirectionEstimate);
    
    if(~bStartPointValid)
        Line1StartingPoint(1,1) = -1;        
        Line1StartingPoint(1,2) = -1;
    end
    
    if(~bEndPointValid)
        Line1EndPoint(1,1) = -1;
        Line1EndPoint(1,2) = -1;
    end
    
end

% 2nd line
if (length(Line2StartingPoint) > 1 || length(Line2EndPoint) > 1)
    [bLine2Found lLine2PointsEq bStartPointValid bEndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,Line2StartingPoint, Line2EndPoint,  bUseDirectionEstimate, InitialDirectionEstimate);
    
    if(~bStartPointValid)
        Line2StartingPoint(1,1) = -1;        
        Line2StartingPoint(1,2) = -1;
    end
    
    if(~bEndPointValid)
        Line2EndPoint(1,1) = -1;
        Line2EndPoint(1,2) = -1;
    end

end

% 3rd line
if (length(Line3StartingPoint) > 1 || length(Line3EndPoint) > 1)
    [bLine3Found lLine3PointsEq bStartPointValid bEndPointValid] = ExtractLine(InegROIFiducial, FiducialEdgeImage,Line3StartingPoint, Line3EndPoint,  bUseDirectionEstimate, InitialDirectionEstimate);
    
    if(~bStartPointValid)
        Line3StartingPoint(1,1) = -1;        
        Line3StartingPoint(1,2) = -1;
    end
    
    if(~bEndPointValid)
        Line3EndPoint(1,1) = -1;
        Line3EndPoint(1,2) = -1;
    end

end


% % recalculate BB locations using line equations & establish corespondence
[BBLocations lLine1PointsEq lLine2PointsEq lLine3PointsEq bLine1Found bLine2Found bLine3Found] = RecalculateBBLocationsAndEstablishCorrespondence(IROIFiducial, BBLocations, lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, bLine1Found, bLine2Found, bLine3Found, Line1StartingPoint, Line2StartingPoint,Line3StartingPoint);

if(bLine1Found)
   bLine1Removed = false;
else
   bLine1Removed = true;
end

if(bLine2Found)
   bLine2Removed = false;
else
   bLine2Removed = true;
end

if(bLine3Found)
   bLine3Removed = false;
else
   bLine3Removed = true;
end
    
NumOfPointsToDefineLine = 5;
PointsOnLine = [];

NumOfPointsToDefineEllipse = 7;
PointsOnEllipse = [];

set(handles.UserAlertMsg, 'string', 'Segmenting ellipses.....');
% ellipse detection
[lEllipse1Points lEllipse2Points] =  ExtractEllipses(InegROIFiducial, lLine2PointsEq, lLine1PointsEq, lLine3PointsEq);

if(length(lEllipse1Points) > 10)
    bEllipse1Removed = false;
else
    bEllipse1Removed = true;
end

if(length(lEllipse2Points) > 10)
    bEllipse2Removed = false;
else
    bEllipse2Removed = true;
end

% segmentation complete, display the results
Floating_BBLocations = BBLocations;
Floating_lLine1PointsEq = lLine1PointsEq;
Floating_lLine2PointsEq = lLine2PointsEq;
Floating_lLine3PointsEq = lLine3PointsEq; 
Floating_lEllipse1Points = lEllipse1Points;
Floating_lEllipse2Points = lEllipse2Points;

DisplayFiducialSegmentationWholeImage;

pause (0.5);

DisplayFiducialSegmentationROI;

iGlobalTaskIdentifier = 0;

FiducialUndoTaskList = [];
set(gcf, 'pointer', 'arrow');
set(handles.UserAlertMsg, 'string', 'Fiducial segmentation complete, press "Accept" or relevant "Add/Delete" buttons to proceed further......');

FiducialSegmentationControlEnable(handles, 'on');



 



% //-----------------------------------------------------------------------------
% // function:	FiducialSegmentationAcceptButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Accept button is clicked
%                   on the GUI, tasks done:
%                   1) disable all other button related to seed
%                   segmentation procedure,
%                   2) copy the floating buffer of segmentation results to
%                   the original/final results buffer
%                   3) Also converts all  the segmentation results,
%                   including seed segmentation results into the image
%                   coordinate system, because so far all the results were
%                   in the corresponding region of interest coordinate
%                   system
% // input:	global variable I (image buffer)
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
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
% varaibles for converting the results to image coordinate system
%             global SeedSegmentationROITranslationVector;
%             global FiducialSegmentationROITranslationVector;
% //output:  
% variables in which the results will be put
% varaible for seed segementation
%             global SeedSegmentationFinalInformationDataSet;
% variable for BB Locations
%             global BBLocations;
% variable for Line equations
%             global lLine1PointsEq;
%             global lLine2PointsEq;
%             global lLine3PointsEq;
% variable for ellipse points
%             global lEllipse1Points; 
%             global lEllipse2Points;
%             Ellipse_1_Eq; 
%             Ellipse_2_Eq;


% // calling function: called when Select region button is pressed
% // NOTE: converts all  the segmentation results,
%                   including seed segmentation results into the image
%                   coordinate system, because so far all the results were
%                   in the corresponding region of interest coordinate
%                   system		
% //-----------------------------------------------------------------------------
% --- Executes on button press in FiducialSegmentationAcceptButton.
function FiducialSegmentationAcceptButton_Callback(hObject, eventdata, handles)
% hObject    handle to FiducialSegmentationAcceptButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global SeedSegmentationFinalInformationDataSet;
global SeedSegmentationROITranslationVector;

global Floating_BBLocations;
global BBLocations;

global Floating_lLine1PointsEq; global Floating_lLine2PointsEq; global Floating_lLine3PointsEq; 
global lLine1PointsEq; global lLine2PointsEq; global lLine3PointsEq; 

global bEllipse1Removed; global bEllipse2Removed;
global Floating_lEllipse1Points; global Floating_lEllipse2Points;
global lEllipse1Points; global lEllipse2Points;

global Segmentation_Complete_Flag;

global bLine1Removed; global bLine2Removed; global bLine3Removed;
global bLine1Found; global bLine2Found; global bLine3Found;
global IROIFiducial;
global Line1StartingPoint;global Line2StartingPoint;global Line3StartingPoint;

global FiducialSegmentationROITranslationVector;

global iGlobalTaskIdentifier; 

global I;

global rufSegmentationData;

rufSegmentationData.FiducialSegmentationPerformed = true;
DoneButtonEnable(handles);

FiducialSegmentationControlToggle(handles, hObject, 0);
iGlobalTaskIdentifier = 0;

set(gcf, 'pointer', 'arrow');

% lines
lLine1PointsEq = Floating_lLine1PointsEq; 
lLine2PointsEq = Floating_lLine2PointsEq;
lLine3PointsEq = Floating_lLine3PointsEq;

% BBs
BBLocations = Floating_BBLocations;

% ellipses
lEllipse1Points = Floating_lEllipse1Points;
lEllipse2Points = Floating_lEllipse2Points; 

% Note: 
%  to allow for fiducial segmentation
%  b4 seed segmentation; only for debug purposes, not the normal flow of
%  program; comment the following two lines of code where results of seed
%  segmentation are accessed


% re-establish correspondence

if(bLine1Removed)
   bLine1Found = false;
else
   bLine1Found = true;
end

if(bLine2Removed)
   bLine2Found = false;
else
   bLine2Found = true;
end

if(bLine3Removed)
   bLine3Found = false;
else
   bLine3Found = true;
end


% % recalculate BB locations using line equations & establish corespondence
[BBLocations lLine1PointsEq lLine2PointsEq lLine3PointsEq bLine1Found bLine2Found bLine3Found] = RecalculateBBLocationsAndEstablishCorrespondence(IROIFiducial, BBLocations, lLine1PointsEq, lLine2PointsEq, lLine3PointsEq, bLine1Found, bLine2Found, bLine3Found, Line1StartingPoint, Line2StartingPoint,Line3StartingPoint);



% display results
imshow(I);

%  display results of seed segmentation
% The parenthesis removed by Ameet, since function was being called without
% any argument and matlab 6.5 does not allow that.
DisplaySeedSegmentationWholeImage;
DisplayFiducialSegmentationWholeImage;



FiducialSegmentationControlEnable(handles, 'off');
set(handles.SeedSelectROIButton, 'enable', 'on');
set(handles.FiducialSelectROIButton, 'enable', 'on');
set(handles.UserAlertMsg, 'string', 'Segmentation results accepted, proceed to 3D reconstruction');



















% //-----------------------------------------------------------------------------
% // function:	AddBBToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
    FiducialSegmentationControlToggle(handles, hObject, 3);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.AddBBToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Add BB active: Click on image to add a BB');
    elseif( button_state == get(hObject,'Min'))
        set(handles.AddBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end










% //-----------------------------------------------------------------------------
% // function:	DeleteBBToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
    FiducialSegmentationControlToggle(handles, hObject, 4);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.DeleteBBToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Delete BB active: Click on image to delete a BB');
    elseif( button_state == get(hObject,'Min'))
        set(handles.DeleteBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end

    



% //-----------------------------------------------------------------------------
% // function:	AddLineToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
% //-----------------------------------------------------------------------------
% --- Executes on button press in AddLineToggleButton.
function AddLineToggleButton_Callback(hObject, eventdata, handles)
% hObject    handle to AddLineToggleButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of AddLineToggleButton
    FiducialSegmentationControlToggle(handles, hObject, 5);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.AddLineToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Add line active: Give 5 points on image to add a line');
    elseif( button_state == get(hObject,'Min'))
        set(handles.AddLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end



% //-----------------------------------------------------------------------------
% // function:	DeleteLineToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
    FiducialSegmentationControlToggle(handles, hObject, 6);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.DeleteLineToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Delete line active: Click on image to delete a line');
    elseif( button_state == get(hObject,'Min'))
        set(handles.DeleteLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end






% //-----------------------------------------------------------------------------
% // function:	AddEllipseToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
    FiducialSegmentationControlToggle(handles, hObject, 7);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.AddEllipseToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Add ellipse active: Give 7 points on image to add an ellipse');
    elseif( button_state == get(hObject,'Min'))
        set(handles.AddEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end




% //-----------------------------------------------------------------------------
% // function:	DeleteEllipseToggleButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
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
    FiducialSegmentationControlToggle(handles, hObject, 8);
    global iGlobalTaskIdentifier;
    button_state = get(hObject,'Value');
    if( button_state == get(hObject,'Max'))
        set(handles.DeleteEllipseToggleButton, 'BackgroundColor', 'white');
        set(handles.UserAlertMsg, 'string', 'Delete ellipse active: Click on image to delete an ellipse');
    elseif( button_state == get(hObject,'Min'))
        set(handles.DeleteEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
    end






% //-----------------------------------------------------------------------------
% // function:	UndoFiducialChangeButton_Callback() 
% // file:		SeedandFiducialSegmentationGUI.m
% // author:		siddharth
% // date:		
% // description:	function is called when Undo button (fiducial segementation group) 
%                   is clicked on the GUI; 
%                   then it does two things: 1) resets the state of all
%                   other toggle buttons to 0
%                   2 ) Retrieves the last action performed by the user
%                   from the FiducialUndoTaskList, e.g. if the last action was
%                   AddBB then it deletes the corresponding BB; & so on
% // input:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global FiducialUndoTaskList;

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
% // output:
% variable that identifies the current task to be performed when user click
% on the image
%             global iGlobalTaskIdentifier; 
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
% variable that manintains the list of tasks done so far by the user in
% LIFO stack form
%             global SeedUndoTaskList;
% // calling function: called when Delete seed toggle button is pressed
% // NOTE: 		
% //-----------------------------------------------------------------------------
% --- Executes on button press in UndoFiducialChangeButton.
function UndoFiducialChangeButton_Callback(hObject, eventdata, handles)
% hObject    handle to UndoFiducialChangeButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global Floating_BBLocations;
global BBLocations;

global bLine1Removed; global bLine2Removed; global bLine3Removed;
global Floating_lLine1PointsEq;global Floating_lLine2PointsEq;global Floating_lLine3PointsEq; 
global lLine1PointsEq;global lLine2PointsEq;global lLine3PointsEq; 

global bEllipse1Removed; global bEllipse2Removed;
global Floating_lEllipse1Points;global Floating_lEllipse2Points;
global lEllipse1Points;global lEllipse2Points;


global FiducialUndoTaskList;
global iGlobalTaskIdentifier;
% SeedUndoTaskList = [TaskId X Y]; note X, Y are in display coordinate
% system, not our coordinate system


% reset the delete pursuit
if (iGlobalTaskIdentifier == 3)
    bAddSeedPursuitOn = false;
    set(handles.AddBBToggleButton, 'Value', 0);
    set(handles.AddBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

% reset the delete pursuit
if (iGlobalTaskIdentifier == 4)
    bDeleteSeedPursuitOn = false;
    set(handles.DeleteBBToggleButton, 'Value', 0);
    set(handles.DeleteBBToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

% reset the delete pursuit
if (iGlobalTaskIdentifier == 5)
    set(handles.AddLineToggleButton, 'Value', 0);
    set(handles.AddLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

        % reset the delete pursuit
if (iGlobalTaskIdentifier == 6)
    set(handles.DeleteLineToggleButton, 'Value', 0);
    set(handles.DeleteLineToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

        % reset the delete pursuit
if (iGlobalTaskIdentifier == 7)
    set(handles.AddEllipseToggleButton, 'Value', 0);
    set(handles.AddEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

        % reset the delete pursuit
if (iGlobalTaskIdentifier == 8)
    set(handles.DeleteEllipseToggleButton, 'Value', 0);
    set(handles.DeleteEllipseToggleButton, 'BackgroundColor', [192/255 192/255 192/255]);
end

iGlobalTaskIdentifier = 0;

set(gcf, 'pointer', 'arrow');

% Logic: pop up the first task, identify the nature of the task
if (length(FiducialUndoTaskList) > 1)
    
    set(gcf, 'pointer', 'watch');
    set (hObject, 'enable', 'off');
    iTaskIdentifier = FiducialUndoTaskList(1,1);
    
    switch(iTaskIdentifier)
        case (3), 
    %       intially added, so undo add i.e. delete/remove a BB
            XToDelete = FiducialUndoTaskList(1,2);
            YToDelete = FiducialUndoTaskList(1,3);
            ToFindBB = [];
            ToFindBB = [YToDelete XToDelete];
            iBBIndex = FindSeed(Floating_BBLocations, ToFindBB);
            Floating_BBLocations (iBBIndex, :) = [-1 -1];    
            DisplayFiducialSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'BB initially added was removed');


        case (4), 
    %       initially deleted, so undo delete i.e. add a seed
            XToAdd = FiducialUndoTaskList(1,2);
            YToAdd = FiducialUndoTaskList(1,3);
            Floating_BBLocations = [Floating_BBLocations; [YToAdd XToAdd]]; 
            hold on;
            plot(XToAdd, YToAdd, 'g.', 'ButtonDownFcn', 'ImageAxesClick_Callback');
            hold off;
            set(handles.UserAlertMsg, 'string', 'BB initially deleted was added back');
            
        case (5),
%           line initially added, need to be deleted
            LineAdded = FiducialUndoTaskList(1,2);
            if(LineAdded == 1)
                Floating_lLine1PointsEq(1) = 0; Floating_lLine1PointsEq(2) = 0; Floating_lLine1PointsEq(3) = 0;   
                bLine1Removed = true;
            else
                if(LineAdded == 2)
                    Floating_lLine2PointsEq(1) = 0; Floating_lLine2PointsEq(2) = 0; Floating_lLine2PointsEq(3) = 0;   
                    bLine2Removed = true;                   
                else
                    if(LineAdded == 3)
                        Floating_lLine3PointsEq(1) = 0; Floating_lLine3PointsEq(2) = 0; Floating_lLine3PointsEq(3) = 0;   
                        bLine3Removed = true;                        
                    end
                end
            end
            DisplayFiducialSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'Line initially added was removed');
            
        case (6),
%            line initially deleted, need to be added
            LineDeleted = FiducialUndoTaskList(1,2);
            if(LineDeleted == 1)
                Floating_lLine1PointsEq = lLine1PointsEq;   
                bLine1Removed = false;
            else
                if(LineDeleted == 2)
                    Floating_lLine2PointsEq = lLine2PointsEq;   
                    bLine2Removed = false;                   
                else
                    if(LineDeleted == 3)
                        Floating_lLine3PointsEq = lLine3PointsEq;   
                        bLine3Removed = false;                        
                    end
                end
            end
            DisplayFiducialSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'Line initially deleted was added back');
            
        case (7),
%           ellipse initially added, need to be deleted
            EllipseAdded = FiducialUndoTaskList(1,2);
            if(EllipseAdded == 1)
                Floating_lEllipse1Points = [];   
                bEllipse1Removed = true;
            else
                if(EllipseAdded == 2)
                    Floating_lEllipse2Points = [];   
                    bEllipse2Removed = true;
                end 
            end
            DisplayFiducialSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'Ellipse initially added was removed');
            
        case (8),
%            ellipse initially deleted, need to be added
            EllipseDeleted = FiducialUndoTaskList(1,2);
            if(EllipseDeleted == 1)
                Floating_lEllipse1Points = lEllipse1Points;   
                bEllipse1Removed = false;
            else
                if(EllipseDeleted == 2)
                    Floating_lEllipse2Points = lEllipse2Points;   
                    bEllipse2Removed = false;                   
                end
            end
            DisplayFiducialSegmentationROI;
            set(handles.UserAlertMsg, 'string', 'Ellipse initially deleted was added back');

    end
    
    FiducialUndoTaskList(1,:) = [];
    set (hObject, 'enable', 'on'); 
    set(gcf, 'pointer', 'arrow');
    
else
    set(handles.UserAlertMsg, 'string', 'No action to undo');
end    


% --- Executes on button press in AbortButton.
function AbortButton_Callback(hObject, eventdata, handles)
% hObject    handle to AbortButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global Segmentation_Complete_Flag;
    Segmentation_Complete_Flag = false;
    delete(handles.SeedandFiducialSegmentationGUI);

    
% --- Executes during object deletion, before destroying properties.
function SeedandFiducialSegmentationGUI_DeleteFcn(hObject, eventdata, handles)
% hObject    handle to SeedandFiducialSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    


% --- Executes when user attempts to close SeedandFiducialSegmentationGUI.
function SeedandFiducialSegmentationGUI_CloseRequestFcn(hObject, eventdata, handles)
% hObject    handle to SeedandFiducialSegmentationGUI (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: delete(hObject) closes the figure
    global Segmentation_Complete_Flag;
    Segmentation_Complete_Flag = false;
    delete(hObject);


% --- Executes on button press in DoneButton.
function DoneButton_Callback(hObject, eventdata, handles)
% hObject    handle to DoneButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% transform the results from Region Of Interest coordinate system to actual Image coordinate system 
% involves addition of translation vector before feeding the points to
% obtain/formulate final equations in image coordinate frame
% 1st the seeds locations

    % seeds data
    global SeedSegmentationFinalInformationDataSet;
    global SeedSegmentationROITranslationVector;

    % fiducial data
    global BBLocations;
    global FiducialSegmentationROITranslationVector;
    global bLine1Found;
    global lLine1PointsEq;
    global bLine2Found;
    global lLine2PointsEq;
    global bLine3Found;
    global lLine3PointsEq;
    global bEllipse1Removed;
    global lEllipse1Points;
    global ellipse1;
    global ellipse_1_eq;
    global bEllipse2Removed;
    global lEllipse2Points;
    global ellipse2;
    global ellipse_2_eq;
    % misc.
    global Segmentation_Complete_Flag;
    
    
    % seeds
    SeedSegmentationFinalInformationDataSet(:,1) = SeedSegmentationFinalInformationDataSet(:,1) + SeedSegmentationROITranslationVector(1,1);
    SeedSegmentationFinalInformationDataSet(:,2) = SeedSegmentationFinalInformationDataSet(:,2) + SeedSegmentationROITranslationVector(1,2);
    
    % fiduacial BBs
    for iBB = 1:9
        if (BBLocations(iBB,1) ~= -1)
            BBLocations(iBB,1) = BBLocations(iBB,1) + FiducialSegmentationROITranslationVector(1,1);
            BBLocations(iBB,2) = BBLocations(iBB,2) + FiducialSegmentationROITranslationVector(1,2);
        end
    end

    % fiducial lines
    if (bLine1Found)
        lLine1PointsEq(3) = lLine1PointsEq(3) - lLine1PointsEq(1)* FiducialSegmentationROITranslationVector(1,1) - lLine1PointsEq(2)* FiducialSegmentationROITranslationVector(1,2);
    else
        lLine1PointsEq = [];
    end
    if(bLine2Found)
        lLine2PointsEq(3) = lLine2PointsEq(3) - lLine2PointsEq(1)* FiducialSegmentationROITranslationVector(1,1) - lLine2PointsEq(2)* FiducialSegmentationROITranslationVector(1,2);
    else
        lLine2PointsEq = [];
    end
    if(bLine3Found)
        lLine3PointsEq(3) = lLine3PointsEq(3) - lLine3PointsEq(1)* FiducialSegmentationROITranslationVector(1,1) - lLine3PointsEq(2)* FiducialSegmentationROITranslationVector(1,2);
    else
        lLine3PointsEq = [];
    end

    % fiducial ellipses
    if (~bEllipse1Removed)
        lEllipse1Points(:,1)  = lEllipse1Points(:,1)+ FiducialSegmentationROITranslationVector(1,1);
        lEllipse1Points(:,2)  = lEllipse1Points(:,2)+ FiducialSegmentationROITranslationVector(1,2);    
        [ellipse1 ellipse_1_eq] = best_fit_ellipse(lEllipse1Points(:,1), lEllipse1Points(:,2));
    else 
        ellipse1 = [];
        ellipse_1_eq = [];
    end

    if(~bEllipse2Removed)
        lEllipse2Points(:,1)  = lEllipse2Points(:,1)+ FiducialSegmentationROITranslationVector(1,1);
        lEllipse2Points(:,2)  = lEllipse2Points(:,2)+ FiducialSegmentationROITranslationVector(1,2);
        [ellipse2 ellipse_2_eq] = best_fit_ellipse(lEllipse2Points(:,1), lEllipse2Points(:,2));
    else
        ellipse2 = [];
        ellipse_2_eq = [];
    end

    Segmentation_Complete_Flag = true;
    delete(handles.SeedandFiducialSegmentationGUI);




% --- Executes on button press in DeleteAllSeedsButton.
function DeleteAllSeedsButton_Callback(hObject, eventdata, handles)
% hObject    handle to DeleteAllSeedsButton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
    global FloatingSeedSegmentationFinalInformationDataSet;
    FloatingSeedSegmentationFinalInformationDataSet = [];    
    DisplaySeedSegmentationROI;


    

% --- Executes on button press in FindClosestPointCheck.
function FindClosestPointCheck_Callback(hObject, eventdata, handles)
% hObject    handle to FindClosestPointCheck (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of FindClosestPointCheck
    global SearchClosestPoint;
    SearchClosestPoint = get(hObject, 'Value');
    SearchClosestPoint


% --- Executes during object creation, after setting all properties.
function FindClosestPointCheck_CreateFcn(hObject, eventdata, handles)
% hObject    handle to FindClosestPointCheck (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called
    global SearchClosestPoint;
    SearchClosestPoint = get(hObject, 'Value');


% $Log: SeedandFiducialSegmentationGUI.m,v $
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



